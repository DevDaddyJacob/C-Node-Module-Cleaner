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
