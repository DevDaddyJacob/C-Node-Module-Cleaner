#ifndef h_state
#define h_state

#include "drive.h"
#include "target.h"
#include "node_module.h"


typedef struct {
    DriveList* drives;
    TargetList* targets;
    NodeModuleList* nodeModules;
} State;


State* getState();

int initState();

int releaseState();

int releaseStateDriveList();

int releaseStateTargetList();

int releaseStateNodeModuleList();

#endif /* h_state */