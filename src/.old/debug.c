#include <stdio.h>

#include "debug.h"


void printTargetList(TargetList* value) {
    printf(
        "TargetList{count:%d, capacity:%d, path:%p}",
        value->count,
        value->capacity,
        *value->path
    );
}

void printDriveList(DriveList* value) {
    printf(
        "DriveList{count:%d, capacity:%d, drive:%p}",
        value->count,
        value->capacity,
        *value->drive
    );
}

void printNodeModuleList(NodeModuleList* value) {
    printf(
        "NodeModuleList{count:%d, capacity:%d, data:%p}",
        value->count,
        value->capacity,
        *value->data
    );
}

void printNodeModule(NodeModule* value) {
    printf(
        "NodeModule{path[ptr -> %p]:\"%s\", drive:'%c', size[ptr -> %p]:%s, lastWriteTime[ptr -> %p]:%s}",
        value->path,
        value->path,
        value->drive,
        value->size,
        uint64_toStringDec(*(value->size)),
        value->lastWriteTime,
        uint64_toStringDec(*(value->lastWriteTime))
    );
}

void printUInt64(UInt64* value) {
    printf(
        "UInt64{low:%d, high:%d}",
        value->low,
        value->high
    );
}