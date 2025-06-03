#ifndef h_sorter
#define h_sorter

#include "node_module.h"

typedef enum {
    SORT_ASCENDING,
    SORT_DESCENDING
} SortDirection;

void sortNodeModuleList(NodeModuleList* list, SortDirection sortDir);

#endif /* h_sorter */