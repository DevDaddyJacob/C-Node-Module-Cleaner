#ifndef h_int_list
#define h_int_list

#include <stdlib.h>

#include "../error.h"
#include "../utils/list.h"


/**
 * \brief           A dynamic array which stores the drives in use in the system
 */
LIST_DEF_STRUCT(int) IntList;

/**
 * \brief           Sets the provided list into an initial zero'ed state
 * \param[in]       list: Pointer to the drive list to zero
 */
LIST_FUNC_SIGNATURE_RESET(IntList);


/**
 * \brief           Creates a new drive list
 * \return          Pointer to the drive list to initialize
 */
LIST_FUNC_SIGNATURE_ALLOC(IntList);

/**
 * \brief           Writes the provided data to the drive list
 * \param[in]       list: Pointer to the drive list write to
 * \param[in]       value: The value to write
 */
LIST_FUNC_SIGNATURE_WRITE(IntList, int);


/**
 * \brief           Releases the provided drive list
 * \param[in]       list: Pointer to the drive list to free
 */
LIST_FUNC_SIGNATURE_FREE(IntList);


/**
 * \brief           Reads the data at the provided index
 */
LIST_FUNC_SIGNATURE_READ(IntList, int);


#endif /* h_int_list */