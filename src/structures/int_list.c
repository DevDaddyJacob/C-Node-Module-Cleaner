#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "int_list.h"
#include "../utils/list.h"

/*
 * ==================================================
 * Function Definitions
 * ==================================================
 */

LIST_FUNCTION_RESET(IntList)

LIST_FUNCTION_ALLOC(IntList)

LIST_FUNCTION_WRITE(IntList, int)

LIST_FUNCTION_FREE(IntList)

LIST_FUNCTION_READ(IntList, int)