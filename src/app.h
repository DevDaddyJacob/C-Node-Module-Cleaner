#ifndef h_app
#define h_app

#include <time.h>
#include <windows.h>

#include "error.h"
#include "./structures/drive_list.h"
#include "./structures/int_list.h"
#include "./structures/node_module.h"
#include "./utils/uint64.h"

typedef enum {
    STAGE_PRE_RUN,

    STAGE_FETCHING_DRIVES,
    STAGE_PENDING_DRIVE_SELECTION,
    STAGE_PENDING_DRIVE_SEARCH,
    STAGE_DRIVES_SEARCHED,

    STAGE_POST_RUN,
} ApplicationStage;

typedef enum {
    THREAD_INACTIVE,
    THREAD_ACTIVE,
    THREAD_COMPLETE,
} AppThreadState;

typedef struct {
    HANDLE handle;
    DWORD id;
    AppThreadState state;
    Error* error;
} ApplicationThread;

typedef struct {
    ApplicationStage stage;
    NodeModuleList* nodeModules;
    DriveList* systemDrives;
    IntList* drivesToScan;
    UInt64 itemsScanned;
    clock_t scanStart;
    clock_t scanEnd;

    ApplicationThread* displayThread;
    ApplicationThread* driveFetchThread;
} ApplicationState;

Error* runApplication();

#endif /* h_app */