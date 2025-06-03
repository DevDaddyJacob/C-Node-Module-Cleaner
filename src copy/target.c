#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "common.h"
#include "target.h"

/*
 * ==================================================
 * Macros
 * ==================================================
 */

#define TARGET_LIST_MIN_SIZE 8

#define TARGET_LIST_GROW_FACTOR 2

#define TARGET_EXPAND_LIST_CAPACITY(currentCapacity) \
    ((currentCapacity) < TARGET_LIST_MIN_SIZE \
        ? TARGET_LIST_MIN_SIZE \
        : (currentCapacity) * TARGET_LIST_GROW_FACTOR)

#define IGNORE_PATH(toIgnore) \
    STR_EQUALS(path, toIgnore)

#define CHECK_IGNORED_PATHS \
    ( \
        IGNORE_PATH("C:\\Windows") \
        || IGNORE_PATH("C:\\Program Files") \
        || IGNORE_PATH("C:\\Program Files (x86)") \
        || IGNORE_PATH("C:\\ProgramData") \
        || IGNORE_PATH("C:\\Recovery") \
        || IGNORE_PATH("C:\\$Recycle.Bin") \
        || IGNORE_PATH("C:\\System Volume Information") \
    )

/*
 * ==================================================
 * Typedefs & Prototypes
 * ==================================================
 */


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

void initTargetList(TargetList* list) {
    list->count = 0;
    list->capacity = 0;
    list->path = NULL;
}

void freeTargetList(TargetList* list) {
    int i;
    
    /* Free each of the strings stored */
    for (i = 0; i < list->count; i++) {
        free(list->path[i]);
    }

    
    /* Free the array's chunk of memory */
    free(list->path);


    /* Reset the list to initial state */
    initTargetList(list);
}

int writeTargetList(TargetList* list, const char* path) {
    char* pathCopy;

    /* Check if we need to expand the array size */
    if (list->count + 1 > list->capacity) {
        int newCapacity;
        char** newArray;

        /* Determine the new capacity of the array */
        newCapacity = TARGET_EXPAND_LIST_CAPACITY(list->capacity);


        /* Attempt to resize the array */
        newArray = (char**)realloc(list->path, sizeof(char*) * newCapacity);
        if (newArray == NULL) {
            return 0; /* Reallocation of array failed, likely out of memory */
        }


        /* If the resize succeeded, place the new array into the struct */
        list->path = newArray;
        list->capacity = newCapacity;
    }


    /* Attempt to allocate memory for the string we are adding */
    pathCopy = (char*)malloc(strlen(path) + 1);
    if (pathCopy == NULL) {
        return 0; /* Allocation of path memory failed, likely out of memory */
    }


    /* Copy the path into our new memory allocation */
    strcpy(pathCopy, path);


    /* Add the path to the end of the array */
    list->path[list->count++] = pathCopy;
    
    return 1;
}

void searchDir(const char* rootPath, TargetList* targetList) {
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
    if (findHandle == INVALID_HANDLE_VALUE) return;


    /* Keep searching the folder */
    do {
        /* If it's a file, ignore it */
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;


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


        /* Check if the path is an ignored item */
        if (CHECK_IGNORED_PATHS) continue;


        /* Check if the path matches our target */
        if (STR_EQUALS(findData.cFileName, "node_modules")) {
            writeTargetList(targetList, path);
            continue;
        }

        searchDir(path, targetList);  /* Recurse into subdirectory */
    } while (FindNextFile(findHandle, &findData));

    FindClose(findHandle);
}