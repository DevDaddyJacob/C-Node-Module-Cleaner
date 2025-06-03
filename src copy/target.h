#ifndef h_target
#define h_target

#include "common.h"

/**
 * \brief           A dynamic array which stores the paths of node_modules
 *                  folders
 */
typedef struct {
    int count;              /** The amount of the elements which are in use */
    int capacity;           /** The amount of elements in the array */
    char** path;            /** Pointer to the first element of the array */
} TargetList;


/**
 * \brief           Initializes the provided target list
 * \param[in]       list: Pointer to the target list to initialize
 */
void initTargetList(TargetList* list);


/**
 * \brief           Releases the provided target list
 * \param[in]       list: Pointer to the target list to free
 */
void freeTargetList(TargetList* list);


/**
 * \brief           Writes the provided path to the target list
 * \note            Output is a boolean treated integer
 * \param[in]       list: Pointer to the list to write to
 * \param[in]       path: The path to write to the list
 * \return          Returns 1 if the write was successful, and 0 otherwise
 */
int writeTargetList(TargetList* list, const char* path);

/**
 * \brief           Searches the provided directory recrusively for targets
 * \param[in]       rootPath: The path to search in
 * \param[in]       targetList: Pointer to the target list to place hits in
 */
void searchDir(const char* rootPath, TargetList* targetList);

#endif /* h_target */