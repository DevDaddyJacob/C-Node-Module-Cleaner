#ifndef h_drive
#define h_drive

/**
 * \brief           A dynamic array which stores the drives in use in the system
 */
typedef struct {
    int count;              /** The amount of the elements which are in use */
    int capacity;           /** The amount of elements in the array */
    char** drive;           /** Pointer to the first element of the array */
} DriveList;


/**
 * \brief           Releases the provided drive list
 * \param[in]       list: Pointer to the drive list to free
 */
void freeDriveList(DriveList* list);


/**
 * \brief           Creates a list of the drives on the system
 * \note            Output is a boolean treated integer
 * \param[out]      list: Pointer to the drive list
 * \return          Returns 1 if the operation was successful, and 0 otherwise
 */
int getSystemDrives(DriveList* list);

#endif /* h_drive */