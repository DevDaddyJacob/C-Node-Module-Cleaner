#ifndef h_node_module
#define h_node_module

#include <windows.h>

#include "target.h"
#include "uint64.h"


typedef struct {
    char* path;                 /* The full path to the folder */
    char drive;                 /* The drive letter the folder is located on */
    UInt64* size;               /* Pointer to the size of the folder */
    UInt64* lastWriteTime;      /* The last time the folder was modified */
} NodeModule;


typedef struct {
    int count;              /** The amount of the elements which are in use */
    int capacity;           /** The amount of elements in the array */
    NodeModule* data;       /** Pointer to the first element of the array */
} NodeModuleList;


/**
 * \brief           Releases the provided list
 * \param[in]       list: Pointer to the list to free
 */
void freeNodeModuleList(NodeModuleList* list);


/**
 * \brief           Created a node module list from the contents of a target
 *                  list.
 * \param[in]       targetList: Pointer to the target list to convert
 * \param[out]      moduleList: Pointer to the module list to use
 */
void fromTargetList(TargetList* targetList, NodeModuleList* moduleList);

#endif /* h_node_module */