#ifndef h_search
#define h_search

#include "target.h"

/**
 * \brief           Searches the provided directory recrusively for targets
 * \param[in]       rootPath: The path to search in
 * \param[in]       targetList: Pointer to the target list to place hits in
 */
void searchDir(const char* rootPath, TargetList* targetList);

#endif /* h_search */