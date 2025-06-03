#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#include "app.h"
#include "error.h"
#include "./structures/drive_list.h"
#include "./structures/int_list.h"
#include "./structures/node_module.h"
#include "./utils/common.h"
#include "./utils/uint64.h"

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

static ApplicationState* initAppState();

static void initThread(ApplicationThread* thread);

static void startThread(
    ApplicationThread* thread, 
    LPTHREAD_START_ROUTINE func
);

void searchDir(const char* rootPath);

void writeToCsv(const char* fileName);

static DWORD WINAPI displayThreadFunc(LPVOID lpParam);

static void displayThread_handleFetchingDrives();

static void displayThread_handleSelectingDrives();

static void displayThread_handleSearchingDrives();


/*
 * ==================================================
 * Module Level Variables & Constants
 * ==================================================
 */

ApplicationState* APP_STATE = NULL;


/*
 * ==================================================
 * Function Definitions
 * ==================================================
 */

static ApplicationState* initAppState() {
    ApplicationState* state = (ApplicationState*)malloc(sizeof(ApplicationState));
    if (state == NULL) {
        exitWithError(createErrorNoCause(
            ERR_MALLOC_FAILED,
            "app.c",
            "initAppState"
        ));

        return; /* Unreachable */
    }
    
    
    /* Allocate & initialize the display thread */
    state->displayThread = (ApplicationThread*)malloc(sizeof(ApplicationThread));
    if (state->displayThread == NULL) {
        free(state);
        
        exitWithError(createErrorNoCause(
            ERR_MALLOC_FAILED,
            "app.c",
            "initAppState"
        ));

        return; /* Unreachable */
    }

    initThread(state->displayThread);
    
    
    /* Allocate & initialize the drive fetch thread */
    state->driveFetchThread = (ApplicationThread*)malloc(sizeof(ApplicationThread));
    if (state->driveFetchThread == NULL) {
        free(state);
        
        exitWithError(createErrorNoCause(
            ERR_MALLOC_FAILED,
            "app.c",
            "initAppState"
        ));

        return; /* Unreachable */
    }

    initThread(state->driveFetchThread);


    /* Default the other fields */
    state->stage = STAGE_PRE_RUN;
    state->systemDrives = NULL;
    state->drivesToScan = NULL;
    state->nodeModules = NULL;
    state->itemsScanned = uint64_fromUInt32(0);
    state->scanStart = 0;
    state->scanEnd = 0;

    return state;
}


static void initThread(ApplicationThread* thread) {
    if (thread == NULL) {
        thread = (ApplicationThread*)malloc(sizeof(ApplicationThread));
        if (thread == NULL) {
            exitWithError(createErrorNoCause(
                ERR_MALLOC_FAILED,
                "app.c",
                "initThread"
            ));

            return; /* Unreachable */
        }
    }

    thread->error = NULL;
    thread->state = THREAD_INACTIVE;
}


static void startThread(
    ApplicationThread* thread, 
    LPTHREAD_START_ROUTINE func
) {
    if (thread == NULL) {
        exitWithError(createErrorNoCause(
            ERR_NULL_POINTER,
            "app.c",
            "startThread"
        ));

        return; /* Unreachable */
    }


    /* Use windows api to create the thread */
    thread->handle = CreateThread(
        NULL,
        0,
        func,
        NULL,
        0,
        &(thread->id)
    );

    if (thread->handle == NULL) {
        exitWithError(createError(
            ERR_APPLICATION,
            "app.c",
            "startThread",
            createErrorNoCause(
                ERR_WIN_CALL_FAILURE,
                "windows.h",
                "CreateThread"
            )
        ));

        return; /* Unreachable */
    }


    /* Update the thread state */
    thread->state = THREAD_ACTIVE;
}


void searchDir(const char* rootPath) {
    char path[MAX_PATH_LENGTH];
    char searchGlob[MAX_PATH_LENGTH];
    WIN32_FIND_DATA findData;
    HANDLE findHandle;


    /* Create search path glob and init the fine operation */
    if (STR_ENDS_WITH_CHAR(rootPath, '\\')) {
        sprintf(searchGlob, "%s*", rootPath);
    } else {
        sprintf(searchGlob, "%s\\*", rootPath);
    }
    findHandle = FindFirstFile(searchGlob, &findData);

    
    /* Handle failure */
    if (findHandle == INVALID_HANDLE_VALUE) return;


    /* Keep searching the folder */
    do {
        /* Increment our metrics counter */
        APP_STATE->itemsScanned = uint64_addUInt32(APP_STATE->itemsScanned, 1);


        /* If it's a file, ignore it */
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;


        /* Check if the path is an relative item */
        if (
            STR_EQUALS(findData.cFileName, ".")
            || STR_EQUALS(findData.cFileName, "..")
        ) continue;


        /* Concatenate the full path */
        if (STR_ENDS_WITH_CHAR(rootPath, '\\')) {
            _snprintf(path, MAX_PATH_LENGTH, "%s%s", rootPath, findData.cFileName);
        } else {
            _snprintf(path, MAX_PATH_LENGTH, "%s\\%s", rootPath, findData.cFileName);
        }


        /* Check if the path is an ignored item */
        if (CHECK_IGNORED_PATHS) continue;


        /* Check if the path matches our target */
        if (STR_EQUALS(findData.cFileName, "node_modules")) {
            writeNodeModuleList(APP_STATE->nodeModules, path);
            continue;
        }

        searchDir(path);  /* Recurse into subdirectory */
    } while (FindNextFile(findHandle, &findData));

    FindClose(findHandle);
}


void writeToCsv(const char* fileName) {
    int i;
    FILE* filePtr;

    /* Open file */
    filePtr = fopen(fileName, "w");


    /* Write the header */
    fprintf(filePtr, "Drive,Size (Bytes),Last Write Time,Path\n");


    /* Write the lines */
    for (i = 0; i < APP_STATE->nodeModules->count; i++) {
        NodeModule* nodeModule = readNodeModuleList(APP_STATE->nodeModules, i);
        fprintf(
            filePtr, 
            "%c,%s,%s,%s\n",
            nodeModule->drive,
            uint64_toStringDec(*getNodeModuleSize(nodeModule)),
            uint64_toStringDec(*getNodeModuleLastWriteTime(nodeModule)),
            nodeModule->path
        );
    }


    /* Close the file */
    fclose(filePtr);
}


static DWORD WINAPI displayThreadFunc(LPVOID lpParam) {
    /* If application is in the pre-run state, wait for it to exit */
    while (APP_STATE->stage == STAGE_PRE_RUN) {
        Sleep(1);
    }


    /* Handle startup drive scanning display */
    displayThread_handleFetchingDrives();


    /* Handle prompting for a selection */
    displayThread_handleSelectingDrives();


    /* Handle scanning display */
    displayThread_handleSearchingDrives();


    return 0;
}


static void displayThread_handleFetchingDrives() {
    int progressDotsCount;
    char* progressDotsStr;

    /* Initialize the variables */
    progressDotsStr = (char*)malloc(sizeof(char) * 4);
    if (progressDotsStr == NULL) {
        APP_STATE->displayThread->error = createError(
            ERR_MALLOC_FAILED,
            "app.c",
            "initThread",
            APP_STATE->displayThread->error
        );

        return;
    }


    /* Handle display */
    progressDotsStr[3] = '\0';
    progressDotsCount = 0;
    while (APP_STATE->stage == STAGE_FETCHING_DRIVES) {
        int i = 0;
        progressDotsCount = progressDotsCount == 3 ? 1 : progressDotsCount + 1;

        for (i = 1; i <= 3; i++) {
            progressDotsStr[i - 1] = i <= progressDotsCount ? '.' : ' ';
        }
        
        reprintf("* Fetching drives%s", progressDotsStr);
        Sleep(750);
    }

    reprintf("* Finished fetching drives");
    printf("\n");
    Sleep(1000);


    /* Free used memory */
    free(progressDotsStr);

    return;
}


static void displayThread_handleSelectingDrives() {
    int currentIndex = 0;
    int i;

    /* Fill the drive state list */
    APP_STATE->drivesToScan = allocDriveList();
    for (i = 0; i < APP_STATE->systemDrives->count; i++) {
        writeIntList(APP_STATE->drivesToScan, 0);
    }


    /* Prime the console by printing the needed lines */
    printf("* Select the drives to scan (Use arrow keys and space to make selections, and confirm with enter)\n");
    for (i = 0; i < APP_STATE->systemDrives->count; i++) {
        printf("\n");
    }


    /* Handle display */
    while (APP_STATE->stage == STAGE_PENDING_DRIVE_SELECTION) {
        SelectionKey key;

        /* Move cursor up to first */
        for (i = 0; i < APP_STATE->systemDrives->count; i++) {
            moveUpLine();
        }


        /* Print each of the options */
        for (i = 0; i < APP_STATE->systemDrives->count; i++) {
            int isSelected = *readIntList(APP_STATE->drivesToScan, i);
            char indicator = isSelected == 1 ? '*' : ' ';
            char leftSelectionIndicator = currentIndex == i ? '>' : ' ';
            char rightSelectionIndicator = currentIndex == i ? '<' : ' ';

            reprintf(
                "  |  %c [%c] %s %c",
                leftSelectionIndicator,
                indicator,
                *readDriveList(APP_STATE->systemDrives, i),
                rightSelectionIndicator
            );

            printf("\n");
        }

        
        /* Scan the next selection key press */
        key = awaitNextSelectionKey();
        switch (key) {
            case SELECT_KEY_UP: {
                if (currentIndex - 1 < 0) {
                    currentIndex = APP_STATE->systemDrives->count - 1;
                } else {
                    currentIndex--;
                }
                break;
            }
            
            case SELECT_KEY_DOWN: {
                if (currentIndex + 1 >= APP_STATE->systemDrives->count) {
                    currentIndex = 0;
                } else {
                    currentIndex++;
                }
                break;
            }

            case SELECT_KEY_SPACE: {
                APP_STATE->drivesToScan->data[currentIndex] = 
                    APP_STATE->drivesToScan->data[currentIndex] == 1 ? 0 : 1;
                break;
            }

            case SELECT_KEY_ENTER: {
                APP_STATE->stage = STAGE_PENDING_DRIVE_SEARCH;
                break;
            }

            case SELECT_KEY_RIGHT: 
            case SELECT_KEY_LEFT: 
            case SELECT_KEY_ESCAPE: break;
        }
    }
}


static void displayThread_handleSearchingDrives() {
    UInt64 lastValue = uint64_fromUInt32(0);
    char* progressDotsStr;
    int refreshes = 20;
    int progressDotsCount = 0;
    double timeElapsed = 0.0;

    /* Initialize the variables */
    progressDotsStr = (char*)malloc(sizeof(char) * 4);
    if (progressDotsStr == NULL) {
        APP_STATE->displayThread->error = createError(
            ERR_MALLOC_FAILED,
            "app.c",
            "displayThread_handleSearchingDrives",
            APP_STATE->displayThread->error
        );

        return;
    }


    /* Prime the console */
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");


    /* Handle display */
    progressDotsStr[3] = '\0';
    progressDotsCount = 0;
    while (APP_STATE->stage == STAGE_PENDING_DRIVE_SEARCH) {
        timeElapsed = (double)(clock() - APP_STATE->scanStart) / CLOCKS_PER_SEC;
        lastValue = APP_STATE->itemsScanned;


        /* Handle progress dot updating */
        if (refreshes >= 20) {
            refreshes = 0;
            int i = 0;
            progressDotsCount = progressDotsCount == 3 ? 1 : progressDotsCount + 1;
    
            for (i = 1; i <= 3; i++) {
                progressDotsStr[i - 1] = i <= progressDotsCount ? '.' : ' ';
            }
        }
        

        /* Handle displays */
        moveUpLine();
        moveUpLine();
        moveUpLine();
        moveUpLine();
        moveUpLine();
        reprintf("* Searching drives%s\n", progressDotsStr);
        reprintf(
            "  | Time Elapsed: %.2f seconds%15c\n",
            timeElapsed,
            ' '
        );
        reprintf(
            "  | Items Scanned: %s%15c\n",
            uint64_toStringDec(lastValue),
            ' '
        );
        reprintf(
            "  | node_modules Found: %d%15c\n",
            APP_STATE->nodeModules->count,
            ' '
        );
        reprintf(
            "  | Scan Speed: %s items / second%15c\n",
            uint64_toStringDec(uint64_divide(
                lastValue,
                uint64_fromUInt32(timeElapsed)
            )),
            ' '
        );

        refreshes++;
        Sleep(25);
    }

    timeElapsed = (double)(APP_STATE->scanEnd - APP_STATE->scanStart) / CLOCKS_PER_SEC;

    moveUpLine();
    moveUpLine();
    moveUpLine();
    moveUpLine();
    moveUpLine();
    reprintf("* Finished Searching Drives\n");
    reprintf(
        "  | Time Elapsed: %.2f seconds%15c\n",
        timeElapsed,
        ' '
    );
    reprintf(
        "  | Items Scanned: %s%15c\n",
        uint64_toStringDec(lastValue),
        ' '
    );
    reprintf(
        "  | node_modules Found: %d%15c\n",
        APP_STATE->nodeModules->count,
        ' '
    );
    reprintf(
        "  | Scan Speed: %s items / second%15c\n",
        uint64_toStringDec(uint64_divide(
            lastValue,
            uint64_fromUInt32(timeElapsed)
        )),
        ' '
    );


    printf("\n");
    Sleep(1000);

    return;
}


Error* runApplication() {
    int i = 0;

    /* Check if the app is already running */
    if (APP_STATE != NULL) return NULL;


    /* Initialize the application's state */
    APP_STATE = initAppState();
    

    /* Start the display thread */
    startThread(APP_STATE->displayThread, displayThreadFunc);


    /* Fetch the drives from the system */
    APP_STATE->stage = STAGE_FETCHING_DRIVES;

    APP_STATE->systemDrives = allocDriveList();
    Error* getDrivesError = getSystemDrives(APP_STATE->systemDrives);
    if (getDrivesError != NULL) {
        return createError(
            ERR_APPLICATION,
            "app.h",
            "runApplication",
            getDrivesError
        );
    }

    Sleep(500);
    APP_STATE->nodeModules = allocNodeModuleList();
    APP_STATE->stage = STAGE_PENDING_DRIVE_SELECTION;


    /* Search the selected drives */
    while (APP_STATE->stage != STAGE_PENDING_DRIVE_SEARCH) Sleep(1);
    APP_STATE->scanStart = clock();

    for (i = 0; i < APP_STATE->systemDrives->count; i++) {
        if ((*readIntList(APP_STATE->drivesToScan, i)) == 0) continue;

        searchDir(*readDriveList(APP_STATE->systemDrives, i));
    }

    APP_STATE->scanEnd = clock();
    APP_STATE->stage = STAGE_DRIVES_SEARCHED;


    writeToCsv("./output.csv");

    
    /* We don't stop until the program is at post run state */
    while (APP_STATE->stage != STAGE_POST_RUN) Sleep(1);

    
    /* Close the threads */
    if (
        APP_STATE->displayThread != NULL 
        && APP_STATE->displayThread->handle != NULL
    ) CloseHandle(APP_STATE->displayThread->handle);
        
    if (
        APP_STATE->driveFetchThread != NULL 
        && APP_STATE->driveFetchThread->handle != NULL
    ) CloseHandle(APP_STATE->driveFetchThread->handle);

    return NULL;
}

