#include <stdio.h>
#include <windows.h>

#include "node_module.h"
#include "common.h"

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
    /*
    NodeModule* nodeModule = (NodeModule*)malloc(sizeof(NodeModule));
    if (nodeModule == NULL) {
        fprintf(stderr, "Error: allocNodeModule exception 1\n");
        return NULL;
    }
    */
    
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

    free(nodeModule);
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
    /* We assume that the pointer is allocated properly */
    char* newPath;
    UInt64* newSize;
    UInt64* newLastWriteTime;

    /* Ensure the node module exists and is of proper size */
    if (nodeModule == NULL) {
        fprintf(stderr, "Error: writeNodeModule exception 1\n");
        return 0;
    }

    if (sizeof(*nodeModule) != sizeof(NodeModule)) {
        fprintf(stderr, "Error: writeNodeModule exception 2\n");
        return 0;
    }


    /* Make a copy of the path */
    newPath = (char*)malloc(strlen(path) + 1);
    if (newPath == NULL) {
        fprintf(stderr, "Error: writeNodeModule exception 3\n");
        return 0; /* Allocation failed, likely out of memory */
    }

    strcpy(newPath, path);

    
    /* Allocate memory for size */
    newSize = (UInt64*)malloc(sizeof(UInt64));
    
    if (newSize == NULL) {
        fprintf(stderr, "Error: writeNodeModule exception 4\n");
        free(newPath);
        return 0; /* Allocation failed, likely out of memory */
    }

    newSize->low = size.low;
    newSize->high = size.high;

    
    /* Allocate memory for last write time */
    newLastWriteTime = (UInt64*)malloc(sizeof(UInt64));
    
    if (newLastWriteTime == NULL) {
        fprintf(stderr, "Error: writeNodeModule exception 5\n");
        free(newPath);
        free(newSize);
        return 0; /* Allocation failed, likely out of memory */
    }

    newLastWriteTime->high = lastWriteTime.dwHighDateTime;
    newLastWriteTime->low = lastWriteTime.dwLowDateTime;


    /* Free the old pointers if they exist */
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


    /* Insert data */
    nodeModule->drive = drive;
    nodeModule->path = newPath;
    nodeModule->size = newSize;
    nodeModule->lastWriteTime = newLastWriteTime;
 
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
        int newCapacity;
        NodeModule* newArray;

        /* Determine the new capacity of the array */
        newCapacity = NODE_MODULE_EXPAND_LIST_CAPACITY(list->capacity);


        /* Attempt to resize the array */
        newArray = (NodeModule*)realloc(list->data, sizeof(NodeModule) * newCapacity);
        if (newArray == NULL) {
            return 0; /* Reallocation of array failed, likely out of memory */
        }


        /* If the resize succeeded, place the new array into the struct */
        list->data = newArray;
        list->capacity = newCapacity;
    }

    /* Create the node module */
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
    initNodeModuleList(moduleList);

    
    /* Loop each of the targets */
    for (i = 0; i < targetList->count; i++) {
        writeNodeModuleList(
            moduleList,
            targetList->path[i],
            targetList->path[i][0],
            UINT64_ZERO,
            (FILETIME){ 0, 0 }
        );
    }
}