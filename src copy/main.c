#include <stdio.h>

#include "drive.h"
#include "target.h"
#include "node_module.h"
#include "uint64.h"
#include "debug.h"
#include "state.h"

int main() {
    State* state = NULL;
    int i = 0;

    /* Initialize the global state */
    if (!initState()) {
        fprintf(stderr, "Fatal: Unable to initialize the global application state");
        releaseState();
        return 1;
    }


    /* Fetch the global state */
    state = getState();
    if (state == NULL) {
        fprintf(stderr, "Fatal: Global application state resolved to an invalid value");
        releaseState();
        return 2;
    }


    /* Initialize the target list */
    initTargetList(state->targets);
    

    /* Try to get the system's drives */
    if (!getSystemDrives(state->drives)) {
        fprintf(stderr, "Fatal: Failed to find system drives");
        releaseState();
        return 3;
    }

    printDriveList(state->drives);
    printf("\n");
    for (i = 0; i < state->drives->count; i++) {
        printf("[%d @ %p] = \"%s\"\n", i, state->drives->drive[i], state->drives->drive[i]);
    }
    printf("\n");


    /* Start searching the drives */
    for (i = 0; i < state->drives->count; i++) {
        searchDir(state->drives->drive[i], state->targets);
    }

    printTargetList(state->targets);
    printf("\n");
    for (i = 0; i < state->targets->count; i++) {
        printf("[%d @ %p] = \"%s\"\n", i, state->targets->path[i], state->targets->path[i]);
    }
    printf("\n\n");

    
    /* Create node module list from target list */
    fromTargetList(state->targets, state->nodeModules);

    printNodeModuleList(state->nodeModules);
    printf("\n");
    for (i = 0; i < state->nodeModules->count; i++) {
        printf("[%d @ %p] = ", i, state->nodeModules->data + i);
        printNodeModule(state->nodeModules->data + i);
        printf("\n");
    }
    printf("\n\n");


    /* Cleanup before exit */
    releaseState();

    return 0;
}