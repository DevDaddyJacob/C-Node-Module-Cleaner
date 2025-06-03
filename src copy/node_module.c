#include <stdio.h>
#include <windows.h>

#include "node_module.h"
#include "common.h"
#include "debug.h"

/*
 * ==================================================
 * Macros
 * ==================================================
 */

#define NODE_MODULE_LIST_MIN_SIZE 8

#define NODE_MODULE_LIST_GROW_FACTOR 2

#define NODE_MODULE_EXPAND_LIST_CAPACITY(currentCapacity) \
    ((currentCapacity) < NODE_MODULE_LIST_MIN_SIZE \
        ? NODE_MODULE_LIST_MIN_SIZE \
        : (currentCapacity) * NODE_MODULE_LIST_GROW_FACTOR)



/*
 * ==================================================
 * Typedefs & Prototypes
 * ==================================================
 */

/**
 * \brief           Initializes the provided list
 * \param[in]       list: Pointer to the list to initialize
 */
static void initNodeModuleList(NodeModuleList* list);


static void initNodeModule(NodeModule* nodeModule);


/**
 * \brief           Writes the provided node module to the list
 * \note            Output is a boolean treated integer
 * \param[in]       list: Pointer to the list to write to
 * \return          Returns 1 if the write was successful, and 0 otherwise
 */
static int writeNodeModuleList(
    NodeModuleList* list,
    const char* path,
    char drive,
    UInt64 size,
    FILETIME lastWriteTime
);

static NodeModule* createNodeModule(
    const char* path,
    char drive,
    UInt64 size,
    FILETIME lastWriteTime
);

static int writeNodeModule(
    NodeModule* nodeModule,
    const char* path,
    char drive,
    UInt64 size,
    FILETIME lastWriteTime
);

static UInt64 countFolderSize(const char* rootPath);

static FILETIME getFolderLastWriteTime(const char* folderPath);


/*
 * ==================================================
 * Module Level Variables & Constants
 * ==================================================
 */

/* int magicNumber = 420; */


/*
 * ==================================================
 * Function Definitions
 * ==================================================
 */

static void initNodeModuleList(NodeModuleList* list) {
    list->count = 0;
    list->capacity = 0;
    list->data = NULL;
}

static void initNodeModule(NodeModule* nodeModule) {
    nodeModule->path = NULL;
    nodeModule->size = NULL;
    nodeModule->lastWriteTime = NULL;
}

static void freeNodeModule(NodeModule* nodeModule) {
    if (nodeModule == NULL) return;
    
    if (nodeModule->path != NULL) {
        free(nodeModule->path);
        nodeModule->path = NULL;
    }
    
    if (nodeModule->size != NULL) {
        free(nodeModule->size);
        nodeModule->size = NULL;
    }
    
    if (nodeModule->lastWriteTime != NULL) {
        free(nodeModule->lastWriteTime);
        nodeModule->lastWriteTime = NULL;
    }
}

static NodeModule* createNodeModule(
    const char* path,
    char drive,
    UInt64 size,
    FILETIME lastWriteTime
) {
    NodeModule* nodeModule;
    
    /* Allocate memory for the node module */
    nodeModule = (NodeModule*)malloc(sizeof(NodeModule));
    if (nodeModule == NULL) {
        fprintf(stderr, "Error: createNodeModule exception 1\n");
        return NULL;
    }


    /* Initialize it to a zero'ed state */
    initNodeModule(nodeModule);


    /* Write the data to the node module */
    if (!writeNodeModule(nodeModule, path, drive, size, lastWriteTime)) {
        fprintf(stderr, "Error: createNodeModule exception 2");
        freeNodeModule(nodeModule);
        return NULL;
    }

    return nodeModule;
}

static int writeNodeModule(
    NodeModule* nodeModule,
    const char* path,
    char drive,
    UInt64 size,
    FILETIME lastWriteTime
) {
    /* printf("DEBUG[writeNodeModule] 1\n"); */
    /* We assume that the pointer is allocated properly */
    char* newPath;
    UInt64* newSize;
    UInt64* newLastWriteTime;

    /* Ensure the node module exists and is of proper size */
    /* printf("DEBUG[writeNodeModule] 2\n"); */
    if (nodeModule == NULL) {
        fprintf(stderr, "Error: writeNodeModule exception 1\n");
        return 0;
    }

    /* printf("DEBUG[writeNodeModule] 3\n"); */
    if (sizeof(*nodeModule) != sizeof(NodeModule)) {
        fprintf(stderr, "Error: writeNodeModule exception 2\n");
        return 0;
    }


    /* Make a copy of the path */
    /* printf("DEBUG[writeNodeModule] 4\n"); */
    newPath = (char*)malloc(strlen(path) + 1);
    if (newPath == NULL) {
        fprintf(stderr, "Error: writeNodeModule exception 3\n");
        return 0; /* Allocation failed, likely out of memory */
    }

    /* printf("DEBUG[writeNodeModule] 5\n"); */
    strcpy(newPath, path);
    /* printf("DEBUG[writeNodeModule] 6\n"); */

    
    /* Allocate memory for size */
    /* printf("DEBUG[writeNodeModule] 7\n"); */
    newSize = (UInt64*)malloc(sizeof(UInt64));
    /* printf("DEBUG[writeNodeModule] 8\n"); */
    
    if (newSize == NULL) {
        fprintf(stderr, "Error: writeNodeModule exception 4\n");
        free(newPath);
        return 0; /* Allocation failed, likely out of memory */
    }

    /* printf("DEBUG[writeNodeModule] 9\n"); */
    newSize->low = size.low;
    newSize->high = size.high;

    
    /* Allocate memory for last write time */
    /* printf("DEBUG[writeNodeModule] 10\n"); */
    newLastWriteTime = (UInt64*)malloc(sizeof(UInt64));
    
    /* printf("DEBUG[writeNodeModule] 11\n"); */
    if (newLastWriteTime == NULL) {
        fprintf(stderr, "Error: writeNodeModule exception 5\n");
        free(newPath);
        free(newSize);
        return 0; /* Allocation failed, likely out of memory */
    }

    /* printf("DEBUG[writeNodeModule] 12\n"); */
    newLastWriteTime->high = lastWriteTime.dwHighDateTime;
    newLastWriteTime->low = lastWriteTime.dwLowDateTime;


    /* Free the old pointers if they exist */
    /* printf("DEBUG[writeNodeModule] 13\n"); */
    if (nodeModule->path != NULL) {
        free(nodeModule->path);
        nodeModule->path = NULL;
    }
    
    /* printf("DEBUG[writeNodeModule] 14\n"); */
    if (nodeModule->size != NULL) {
        free(nodeModule->size);
        nodeModule->size = NULL;
    }
    
    /* printf("DEBUG[writeNodeModule] 15\n"); */
    if (nodeModule->lastWriteTime != NULL) {
        free(nodeModule->lastWriteTime);
        nodeModule->lastWriteTime = NULL;
    }


    /* Insert data */
    /* printf("DEBUG[writeNodeModule] 16\n"); */
    nodeModule->drive = drive;
    nodeModule->path = newPath;
    nodeModule->size = newSize;
    nodeModule->lastWriteTime = newLastWriteTime;
    /* printf("DEBUG[writeNodeModule] 17\n"); */
 
    return 1;
}

static int writeNodeModuleList(
    NodeModuleList* list,
    const char* path,
    char drive,
    UInt64 size,
    FILETIME lastWriteTime
) {
    /* Check if we need to expand the array size */
    if (list->count + 1 > list->capacity) {
        /* printf("DEBUG[writeNodeModuleList] 1.1\n"); */
        int newCapacity;
        NodeModule* newArray;

        /* Determine the new capacity of the array */
        newCapacity = NODE_MODULE_EXPAND_LIST_CAPACITY(list->capacity);
        /* printf("DEBUG[writeNodeModuleList] 1.2\n"); */


        /* Attempt to resize the array */
        /* printf("DEBUG[writeNodeModuleList] 1.3\n"); */
        if (list->data == NULL) {
            /* printf("DEBUG[writeNodeModuleList] 1.3.1\n"); */
            newArray = (NodeModule*)malloc(sizeof(NodeModule) * newCapacity);
        } else {
            /* printf("DEBUG[writeNodeModuleList] 1.3.2\n"); */
            newArray = (NodeModule*)realloc(list->data, sizeof(NodeModule) * newCapacity);
        }

        /* printf("DEBUG[writeNodeModuleList] 1.4\n"); */
        if (newArray == NULL) {
        /* printf("DEBUG[writeNodeModuleList] 1.4.1\n"); */
            return 0; /* Reallocation of array failed, likely out of memory */
        }


        /* If the resize succeeded, place the new array into the struct */
        /* printf("DEBUG[writeNodeModuleList] 1.5\n"); */
        list->data = newArray;
        list->capacity = newCapacity;
        /* printf("DEBUG[writeNodeModuleList] 1.6\n"); */
    }


    /* Initialize the node module */
    /* printf("DEBUG[writeNodeModuleList] 2\n"); */
    initNodeModule(list->data + list->count);


    /* Create the node module */
    /* printf("DEBUG[writeNodeModuleList] 3\n"); */
    if (
        !writeNodeModule(
            list->data + list->count, 
            path, 
            drive, 
            size, 
            lastWriteTime
        )
    ) {
        return 0;
    }
    
    list->count++;
    
    return 1;
}

static NodeModule* copyNodeModule(NodeModule* original) {
    return createNodeModule(
        original->path,
        original->drive,
        *(original->size),
        (FILETIME){
            original->lastWriteTime->low,
            original->lastWriteTime->high
        }
    );
}

static UInt64 countFolderSize(const char* rootPath) {
    UInt64 folderSize = uint64_fromUInt32(0);
    char path[MAX_PATH_LENGTH];
    char searchGlob[MAX_PATH_LENGTH];
    WIN32_FIND_DATA findData;
    HANDLE findHandle;


    /* Create search path glob and init the fine operation */
    if (STR_ENDS_WITH_CHAR(rootPath, '\\')) {
        sprintf(searchGlob, "%s*", rootPath);
    } else {
        sprintf(searchGlob, "%s\\*", rootPath);
    }
    findHandle = FindFirstFile(searchGlob, &findData);

    
    /* Handle failure */
    if (findHandle == INVALID_HANDLE_VALUE) return folderSize;


    /* Keep searching the folder */
    do {
        UInt64 fileSize;

        /* Check if the path is an relative item */
        if (
            STR_EQUALS(findData.cFileName, ".")
            || STR_EQUALS(findData.cFileName, "..")
        ) continue;


        /* Concatenate the full path */
        if (STR_ENDS_WITH_CHAR(rootPath, '\\')) {
            _snprintf(path, MAX_PATH_LENGTH, "%s%s", rootPath, findData.cFileName);
        } else {
            _snprintf(path, MAX_PATH_LENGTH, "%s\\%s", rootPath, findData.cFileName);
        }


        /* Calculate the size */
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            fileSize = countFolderSize(path);
        } else {
            fileSize = uint64_fromUInt32(
                (findData.nFileSizeHigh * (MAXDWORD + 1)) + findData.nFileSizeLow
            );
        }

        folderSize = uint64_add(folderSize, fileSize);
    } while (FindNextFile(findHandle, &findData));

    FindClose(findHandle);
    return folderSize;
}

static FILETIME getFolderLastWriteTime(const char* folderPath) {
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;

    if (GetFileAttributesEx(folderPath, GetFileExInfoStandard, &fileInfo)) {
        return fileInfo.ftLastWriteTime;
    }

    return (FILETIME){ 0, 0 };
}

void freeNodeModuleList(NodeModuleList* list) {
    int i;
    
    /* Free each of the node modules stored */
    for (i = 0; i < list->count; i++) {
        freeNodeModule(list->data + i);
    }

    
    /* Free the array's chunk of memory */
    free(list->data);


    /* Reset the list to initial state */
    initNodeModuleList(list);
}

void fromTargetList(TargetList* targetList, NodeModuleList* moduleList) {
    int i;

    /* Initialize the list to an empty state */
    /* printf("DEBUG[fromTargetList] Initializing list\n"); */
    initNodeModuleList(moduleList);

    
    /* Loop each of the targets */
    /* printf("DEBUG[fromTargetList] Looping list\n"); */
    for (i = 0; i < targetList->count; i++) {
        /* printf("DEBUG[fromTargetList] Loop index %d\n", i); */
        writeNodeModuleList(
            moduleList,
            targetList->path[i],
            targetList->path[i][0],
            countFolderSize(targetList->path[i]),
            getFolderLastWriteTime(targetList->path[i])
        );
    }
}