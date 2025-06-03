#ifndef h_node_module
#define h_node_module

#include "../utils/uint64.h"
#include "../utils/list.h"

typedef struct {
    char* path;
    char drive;
    UInt64* size;
    UInt64* lastWriteTime;
} NodeModule;

LIST_DEF_STRUCT(NodeModule) NodeModuleList;

LIST_FUNC_SIGNATURE_RESET(NodeModuleList);

LIST_FUNC_SIGNATURE_ALLOC(NodeModuleList);

void writeNodeModuleList(NodeModuleList* list, const char* path);

LIST_FUNC_SIGNATURE_FREE(NodeModuleList);

LIST_FUNC_SIGNATURE_READ(NodeModuleList, NodeModule);

UInt64* getNodeModuleSize(NodeModule* nodeModule);

UInt64* getNodeModuleLastWriteTime(NodeModule* nodeModule);

#endif /* h_node_module */