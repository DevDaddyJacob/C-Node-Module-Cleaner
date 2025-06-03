#ifndef h_drive_list
#define h_drive_list

#include <stdlib.h>

#include "../error.h"
#include "../utils/list.h"


/**
 * \brief           A dynamic array which stores the drives in use in the system
 */
LIST_DEF_STRUCT(char*) DriveList;


/**
 * \brief           Creates a new drive list
 * \return          Pointer to the drive list to initialize
 */
LIST_FUNC_SIGNATURE_ALLOC(DriveList);


/**
 * \brief           Releases the provided drive list
 * \param[in]       list: Pointer to the drive list to free
 */
LIST_FUNC_SIGNATURE_FREE(DriveList);


/**
 * \brief           Reads the data at the provided index
 */
LIST_FUNC_SIGNATURE_READ(DriveList, char*);


/**
 * \brief           Creates a list of the drives on the system
 * \note            Output is a boolean treated integer
 * \param[out]      list: Pointer to the drive list
 * \return          Returns the error encountered, or NULL otherwise
 */
Error* getSystemDrives(DriveList* list);


#endif /* h_drive_list */