#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "common.h"
#include "search.h"

/*
 * ==================================================
 * Macros
 * ==================================================
 */

#define IGNORE_PATH(toIgnore) \
    STR_EQUALS(path, toIgnore)

#define CHECK_IGNORED_PATHS \
    ( \
           IGNORE_PATH("C:\\Windows") \
        || IGNORE_PATH("C:\\Program Files") \
        || IGNORE_PATH("C:\\Program Files (x86)") \
        || IGNORE_PATH("C:\\ProgramData") \
        || IGNORE_PATH("C:\\Recovery") \
        || IGNORE_PATH("C:\\$Recycle.Bin") \
        || IGNORE_PATH("C:\\System Volume Information") \
    )


/*
 * ==================================================
 * Typedefs & Prototypes
 * ==================================================
 */



/*
 * ==================================================
 * Module Level Variables & Constants
 * ==================================================
 */

/* int magicNumber = 420; */


/*
 * ==================================================
 * Function Definitions
 * ==================================================
 */

void searchDir(const char* rootPath, TargetList* targetList) {
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
            writeTargetList(targetList, path);
            continue;
        }

        searchDir(path, targetList);  /* Recurse into subdirectory */
    } while (FindNextFile(findHandle, &findData));

    FindClose(findHandle);
}
