#ifndef h_debug
#define h_debug

#include "drive.h"
#include "target.h"
#include "node_module.h"
#include "uint64.h"

void printTargetList(TargetList* value);
void printDriveList(DriveList* value);
void printNodeModuleList(NodeModuleList* value);
void printNodeModule(NodeModule* value);
void printUInt64(UInt64* value);


#endif /* h_debug */