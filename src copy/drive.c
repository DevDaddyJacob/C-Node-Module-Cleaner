#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "drive.h"

/*
 * ==================================================
 * Macros
 * ==================================================
 */

#define DRIVE_LIST_MIN_SIZE 1

#define DRIVE_LIST_GROW_FACTOR 2

#define DRIVE_EXPAND_LSIT_CAPACITY(currentCapacity) \
    ((currentCapacity) < DRIVE_LIST_MIN_SIZE \
        ? DRIVE_LIST_MIN_SIZE \
        : (currentCapacity) * DRIVE_LIST_GROW_FACTOR)

/*
 * ==================================================
 * Typedefs & Prototypes
 * ==================================================
 */

/**
 * \brief           Initializes the provided drive list
 * \param[in]       list: Pointer to the drive list to initialize
 */
static void initDriveList(DriveList* list);

/**
 * \brief           Writes the provided string to the drive list
 * \note            Output is a boolean treated integer
 * \param[in]       list: Pointer to the list to write to
 * \param[in]       data: The string to write to the list
 * \return          Returns 1 if the write was successful, and 0 otherwise
 */
static int writeDriveList(DriveList* list, const char* data);


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

static void initDriveList(DriveList* list) {
    list->count = 0;
    list->capacity = 0;
    list->drive = NULL;
}

static int writeDriveList(DriveList* list, const char* data) {
    char* dataCopy;

    /* Check if we need to expand the array size */
    if (list->count + 1 > list->capacity) {
        int newCapacity;
        char** newArray;

        /* Determine the new capacity of the array */
        newCapacity = DRIVE_EXPAND_LSIT_CAPACITY(list->capacity);


        /* Attempt to resize the array */
        newArray = (char**)realloc(list->drive, sizeof(char*) * newCapacity);
        if (newArray == NULL) {
            return 0; /* Reallocation of array failed, likely out of memory */
        }


        /* If the resize succeeded, place the new array into the struct */
        list->drive = newArray;
        list->capacity = newCapacity;
    }


    /* Attempt to allocate memory for the string we are adding */
    dataCopy = (char*)malloc(strlen(data) + 1);
    if (dataCopy == NULL) {
        return 0; /* Allocation of data memory failed, likely out of memory */
    }


    /* Copy the data into our new memory allocation */
    strcpy(dataCopy, data);


    /* Add the data to the end of the array */
    list->drive[list->count++] = dataCopy;
    
    return 1;
}

void freeDriveList(DriveList* list) {
    int i;
    
    /* Free each of the strings stored */
    for (i = 0; i < list->count; i++) {
        free(list->drive[i]);
    }

    
    /* Free the array's chunk of memory */
    free(list->drive);


    /* Reset the list to initial state */
    initDriveList(list);
}

int getSystemDrives(DriveList* list) {
    char buffer[256];
    DWORD result;
    char* drive;

    /* Place the list into an initialized state */
    initDriveList(list);


    /* Use the windows call to place the drives into the buffer */
    result = GetLogicalDriveStrings(sizeof(buffer), buffer);
    if (result == 0 || result > sizeof(buffer)) {
        fprintf(
            stderr, 
            "Error: Encountered an error when making call to 'GetLogicalDriveStrings'"
        );
        return 0;
    }


    /* Read each of the drives and add to the list */
    drive = buffer;
    while (*drive) {
        /* Write the drive to the list */
        if (!writeDriveList(list, drive)) {
            fprintf(
                stderr, 
                "Error: Encountered an error when trying to write to instance of DriveList"
            );
            return 0;
        }

        /* Move the pointer to the next drive */
        drive += lstrlen(drive) + 1;
    }

    return 1;
}