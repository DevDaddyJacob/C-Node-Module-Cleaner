#include <stdio.h>
#include <stdlib.h>

#include "state.h"
#include "drive.h"
#include "target.h"
#include "node_module.h"

/*
 * ==================================================
 * Macros
 * ==================================================
 */


/*
 * ==================================================
 * Typedefs & Prototypes
 * ==================================================
 */

typedef enum {
    STATE_INITIALIZED,
    STATE_INITIALIZING,
    STATE_UNINITIALIZED,
} StateStatus;

/*
 * ==================================================
 * Module Level Variables & Constants
 * ==================================================
 */

StateStatus GLOBAL_STATE_STATUS = STATE_UNINITIALIZED;
State* GLOBAL_STATE = NULL;


/*
 * ==================================================
 * Function Definitions
 * ==================================================
 */

State* getState() {
    /* Check if the state is initialized */
    if (GLOBAL_STATE_STATUS != STATE_INITIALIZED) return NULL;

    return GLOBAL_STATE;
}

int initState() {
    /* If we already have the state setup, stop now */
    if (GLOBAL_STATE_STATUS != STATE_UNINITIALIZED) return 0;
    GLOBAL_STATE_STATUS = STATE_INITIALIZING;
    
    /* Allocate memory for the state */
    GLOBAL_STATE = (State*)malloc(sizeof(State));
    if (GLOBAL_STATE == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for global state!");
        return 0;
    }


    /* Zero / Null out the fields */
    GLOBAL_STATE->drives = NULL;
    GLOBAL_STATE->targets = NULL;
    GLOBAL_STATE->nodeModules = NULL;


    /* Allocate memory for the drives list */
    GLOBAL_STATE->drives = (DriveList*)malloc(sizeof(DriveList));
    if (GLOBAL_STATE->drives == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for drive list!");
        releaseState();
        return 0;
    }


    /* Allocate memory for the target list */
    GLOBAL_STATE->targets = (TargetList*)malloc(sizeof(TargetList));
    if (GLOBAL_STATE->targets == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for target list!");
        releaseState();
        return 0;
    }


    /* Allocate memory for the node modules list */
    GLOBAL_STATE->nodeModules = (NodeModuleList*)malloc(sizeof(NodeModuleList));
    if (GLOBAL_STATE->nodeModules == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for node modules list!");
        releaseState();
        return 0;
    }

    GLOBAL_STATE_STATUS = STATE_INITIALIZED;
    return 1;
}

int releaseState() {
    int success = 1;

    /* If we already have the state torn down, stop now */
    if (
        GLOBAL_STATE_STATUS != STATE_INITIALIZED
        && GLOBAL_STATE_STATUS != STATE_INITIALIZING
    ) return 0;


    /* Release memory for each field */
    if (!releaseStateDriveList()) success = 0;
    if (!releaseStateTargetList()) success = 0;
    if (!releaseStateNodeModuleList()) success = 0;


    /* Zero / Null each of the fields */
    GLOBAL_STATE->drives = NULL;
    GLOBAL_STATE->targets = NULL;
    GLOBAL_STATE->nodeModules = NULL;

    
    /* Release the memory for the state itself */
    free(GLOBAL_STATE);
    GLOBAL_STATE = NULL;

    GLOBAL_STATE_STATUS = STATE_UNINITIALIZED;
    return success;
}

int releaseStateDriveList() {
    /* If the state isn't initialized(ing), we cannot gaurentee valid pointers */
    if (
        GLOBAL_STATE_STATUS != STATE_INITIALIZED
        && GLOBAL_STATE_STATUS != STATE_INITIALIZING
    ) return 0;


    /* Ensure the state has a pointer */
    if (GLOBAL_STATE == NULL) return 0;


    /* Check if the item is already nulled out */
    if (GLOBAL_STATE->drives == NULL) return 1;


    /* Release the item's memory */
    freeDriveList(GLOBAL_STATE->drives);
    GLOBAL_STATE->drives = NULL;
    
    return 1;
}

int releaseStateTargetList() {
    /* If the state isn't initialized(ing), we cannot gaurentee valid pointers */
    if (
        GLOBAL_STATE_STATUS != STATE_INITIALIZED
        && GLOBAL_STATE_STATUS != STATE_INITIALIZING
    ) return 0;


    /* Ensure the state has a pointer */
    if (GLOBAL_STATE == NULL) return 0;


    /* Check if the item is already nulled out */
    if (GLOBAL_STATE->targets == NULL) return 1;


    /* Release the item's memory */
    freeTargetList(GLOBAL_STATE->targets);
    GLOBAL_STATE->targets = NULL;
    
    return 1;
}

int releaseStateNodeModuleList() {
    /* If the state isn't initialized(ing), we cannot gaurentee valid pointers */
    if (
        GLOBAL_STATE_STATUS != STATE_INITIALIZED
        && GLOBAL_STATE_STATUS != STATE_INITIALIZING
    ) return 0;


    /* Ensure the state has a pointer */
    if (GLOBAL_STATE == NULL) return 0;


    /* Check if the item is already nulled out */
    if (GLOBAL_STATE->nodeModules == NULL) return 1;


    /* Release the item's memory */
    freeNodeModuleList(GLOBAL_STATE->nodeModules);
    GLOBAL_STATE->nodeModules = NULL;
    
    return 1;
}