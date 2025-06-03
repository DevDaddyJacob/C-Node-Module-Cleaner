#include <string.h>
#include <stdlib.h>
#include <windows.h>

#include "node_module.h"
#include "../error.h"
#include "../utils/common.h"
#include "../utils/list.h"

/*
 * ==================================================
 * Macros
 * ==================================================
 */



/*
 * ==================================================
 * Typedefs & Prototypes
 * ==================================================
 */

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

LIST_FUNCTION_RESET(NodeModuleList)

LIST_FUNCTION_ALLOC(NodeModuleList)

void writeNodeModuleList(NodeModuleList* list, const char* path) {
    int i = 0;
    NodeModule* nodeModule = NULL;

    if (list->capacity < list->count + 1) { 
        list->capacity = (list->capacity) < 1 ? 1 : list->capacity * 2;
        list->data = realloc(list->data, sizeof(NodeModule) * list->capacity); 
    }
    
    nodeModule = list->data + list->count;
    list->count++;
    
    /* Zero out the fields */
    nodeModule->path = NULL;
    nodeModule->drive = NULL;
    nodeModule->size = NULL;
    nodeModule->lastWriteTime = NULL;


    /* Copy the string into the path */
    nodeModule->path = (char*)malloc(strlen(path) + 1);
    if (nodeModule->path == NULL) {
        exitWithError(createErrorNoCause(
            ERR_MALLOC_FAILED,
            "node_module.h",
            "allocNodeModule"
        ));

        return NULL; /* Unreachable */
    }

    strcpy(nodeModule->path, path);


    /* Write the drive */
    nodeModule->drive = path[0];
}

LIST_FUNCTION_FREE(NodeModuleList)

LIST_FUNCTION_READ(NodeModuleList, NodeModule)

UInt64* getNodeModuleSize(NodeModule* nodeModule) {
    /* Sanity check */
    if (nodeModule == NULL) {
        exitWithError(createErrorNoCause(
            ERR_NULL_POINTER,
            "node_module.h",
            "getNodeModuleSize"
        ));

        return NULL; /* Unreachable */
    }

    
    /* If the size is already attached return that */
    if (nodeModule->size != NULL) {
        return nodeModule->size;
    }


    /* Allocate memory for the size */
    nodeModule->size = (UInt64*)malloc(sizeof(UInt64));
    if (nodeModule->size == NULL) {
        exitWithError(createErrorNoCause(
            ERR_MALLOC_FAILED,
            "node_module.h",
            "getNodeModuleSize"
        ));

        return NULL; /* Unreachable */
    }


    /* Count the folder size */
    *(nodeModule->size) = countFolderSize(nodeModule->path);

    return nodeModule->size;
}

UInt64* getNodeModuleLastWriteTime(NodeModule* nodeModule) {
    FILETIME fileTime;

    /* Sanity check */
    if (nodeModule == NULL) {
        exitWithError(createErrorNoCause(
            ERR_NULL_POINTER,
            "node_module.h",
            "getNodeModuleLastWriteTime"
        ));

        return NULL; /* Unreachable */
    }

    
    /* If the size is already attached return that */
    if (nodeModule->lastWriteTime != NULL) {
        return nodeModule->lastWriteTime;
    }


    /* Allocate memory for the size */
    nodeModule->lastWriteTime = (UInt64*)malloc(sizeof(UInt64));
    if (nodeModule->lastWriteTime == NULL) {
        exitWithError(createErrorNoCause(
            ERR_MALLOC_FAILED,
            "node_module.h",
            "getNodeModuleLastWriteTime"
        ));

        return NULL; /* Unreachable */
    }


    /* Count the folder size */
    fileTime = getFolderLastWriteTime(nodeModule->path);
    nodeModule->lastWriteTime->low = fileTime.dwLowDateTime;
    nodeModule->lastWriteTime->high = fileTime.dwHighDateTime;

    return nodeModule->lastWriteTime;
}

