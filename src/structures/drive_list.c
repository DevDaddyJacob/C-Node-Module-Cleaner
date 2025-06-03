#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "drive_list.h"
#include "../error.h"
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

/**
 * \brief           Sets the provided list into an initial zero'ed state
 * \param[in]       list: Pointer to the drive list to zero
 */
static LIST_FUNC_SIGNATURE_RESET(DriveList);

/**
 * \brief           Writes the provided data to the drive list
 * \param[in]       list: Pointer to the drive list write to
 * \param[in]       value: The value to write
 */
static LIST_FUNC_SIGNATURE_WRITE(DriveList, char*);

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

static LIST_FUNCTION_RESET(DriveList)

static LIST_FUNCTION_WRITE(DriveList, char*)

LIST_FUNCTION_ALLOC(DriveList)

LIST_FUNCTION_FREE(DriveList)

LIST_FUNCTION_READ(DriveList, char*)

Error* getSystemDrives(DriveList* list) {
    char buffer[256];
    DWORD result;
    char* drive;
    
    /* Zero the list */
    if (list == NULL) {
        return createErrorNoCause(
            ERR_NULL_POINTER,
            "drive_list.h",
            "getSystemDrives"
        );
    }
    
    resetDriveList(list);


    /* Use the windows call to place the drives into the buffer */
    result = GetLogicalDriveStrings(sizeof(buffer), buffer);
    if (result == 0 || result > sizeof(buffer)) {
        Error* winError = createErrorNoCause(
            ERR_WIN_CALL_FAILURE,
            "windows.h",
            "GetLogicalDriveStrings"
        );

        return createError(
            ERR_APPLICATION,
            "drive_list.h",
            "getSystemDrives",
            winError
        );
    }


    /* Read each of the drives and add to the list */
    drive = buffer;
    while (*drive) {
        /* Allocate a spot in memory for the drive string */
        char* driveStr = (char*)malloc(strlen(drive) + 1);
        if (driveStr == NULL) {
            return createErrorNoCause(
                ERR_MALLOC_FAILED,
                "drive_list.h",
                "getSystemDrives"
            );
        }
        

        /* Copy the string */
        strcpy(driveStr, drive);


        /* Write the drive to the list */
        writeDriveList(list, driveStr);


        /* Move the pointer to the next drive */
        drive += lstrlen(drive) + 1;
    }


    /* Check if we have any system drives */
    if (list->count == 0) {
        return createErrorNoCause(
            ERR_NO_DRIVES_FOUND,
            "drive_list.h",
            "getSystemDrives"
        );
    }

    return NULL;
}
