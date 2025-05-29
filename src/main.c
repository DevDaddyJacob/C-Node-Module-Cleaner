#include <stdio.h>

#include "drive.h"
#include "target.h"
#include "search.h"
#include "node_module.h"


int main() {
    int i = 0;
    DriveList* drives = NULL;
    TargetList* targets = NULL;
    NodeModuleList* nodeModules = NULL;

    /* Allocate memory for the drives list */
    drives = (DriveList*)malloc(sizeof(DriveList));
    if (drives == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for drive list");
        return 1;
    }


    /* Allocate memory for the target list */
    targets = (TargetList*)malloc(sizeof(TargetList));
    if (targets == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for target list");

        freeDriveList(drives);
        return 1;
    }


    /* Allocate memory for the node modules list */
    nodeModules = (NodeModuleList*)malloc(sizeof(NodeModuleList));
    if (nodeModules == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for node modules list");

        freeDriveList(drives);
        freeTargetList(targets);
        return 1;
    }


    /* Initialize the target list */
    initTargetList(targets);
    

    /* Try to get the system's drives */
    if (!getSystemDrives(drives)) {
        fprintf(stderr, "Error: Failed to find system drives");

        freeDriveList(drives);
        freeTargetList(targets);
        freeNodeModuleList(nodeModules);
        return 1;
    }

    printf(
        "DriveList{count:%d, capacity:%d, drive:%p}\n",
        drives->count,
        drives->capacity,
        *drives->drive
    );
    for (i = 0; i < drives->count; i++) {
        printf("[%d @ %p] = \"%s\"\n", i, drives->drive[i], drives->drive[i]);
    }
    printf("\n");


    /* Start searching the drives */
    searchDir(drives->drive[0], targets);

    printf(
        "TargetList{count:%d, capacity:%d, path:%p}\n",
        targets->count,
        targets->capacity,
        *targets->path
    );
    for (i = 0; i < targets->count; i++) {
        printf("[%d @ %p] = \"%s\"\n", i, targets->path[i], targets->path[i]);
    }
    printf("\n\n");

    
    /* Create node module list from target list */
    fromTargetList(targets, nodeModules);

    printf(
        "NodeModuleList{count:%d, capacity:%d, data:%p}\n",
        nodeModules->count,
        nodeModules->capacity,
        *nodeModules->data
    );
    for (i = 0; i < nodeModules->count; i++) {
        printf("[%d @ %p] = \n", i, nodeModules->data[i]);
    }
    printf("\n\n");




    /* Cleanup before exit */
    freeDriveList(drives);
    freeTargetList(targets);
    freeNodeModuleList(nodeModules);
    return 0;
}