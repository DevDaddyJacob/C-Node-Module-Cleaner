#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

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

static void printErrorWithoutCause(Error* error);


/*
 * ==================================================
 * Module Level Variables & Constants
 * ==================================================
 */

const char* ERROR_NAMES[] = {
    "ERR_UNKNOWN",              /* ERR_UNKNOWN */
    "ERR_APPLICATION",          /* ERR_APPLICATION */
    "ERR_NULL_POINTER",         /* ERR_NULL_POINTER */
    "ERR_MALLOC_FAILED",        /* ERR_MALLOC_FAILED */
    "ERR_WIN_CALL_FAILURE",     /* ERR_WIN_CALL_FAILURE */
    "ERR_NO_DRIVES_FOUND",      /* ERR_NO_DRIVES_FOUND */
};

const char* ERROR_MESSAGES[] = {
    "Encountered an unknown error",                                     /* ERR_UNKNOWN */
    "Encountered an application error",                                 /* ERR_APPLICATION */
    "A pointer value was NULL when it definitely shouldn't have been",  /* ERR_NULL_POINTER */
    "Memory allocation via 'malloc' call failed",                       /* ERR_MALLOC_FAILED */
    "A call to a windows function failed",                              /* ERR_WIN_CALL_FAILURE */
    "No drives were found on the system",                               /* ERR_NO_DRIVES_FOUND */
};


/*
 * ==================================================
 * Function Definitions
 * ==================================================
 */

static void printErrorWithoutCause(Error* error) {
    fprintf(
        stderr, 
        "%s [%#04x]: %s\n\tin method '%s' in module '%s'\n",
        ERROR_NAMES[error->code],
        error->code,
        ERROR_MESSAGES[error->code],
        error->method,
        error->module
    );
}

Error* createError(
    ErrorCode code,
    const char* module,
    const char* method,
    Error* cause
) {
    Error* newError;

    /* Allocate memory for the new error */
    newError = (Error*)malloc(sizeof(Error));
    if (newError == NULL) return NULL;

    
    /* Assign the code and cause */
    newError->code = code;
    newError->cause = cause;


    /* Copy the module */
    newError->module = (char*)malloc(strlen(module) + 1);
    if (newError->module == NULL) {
        free(newError);
        return NULL;
    }

    strcpy(newError->module, module);


    /* Copy the method */
    newError->method = (char*)malloc(strlen(method) + 1);
    if (newError->method == NULL) {
        free(newError->module);
        free(newError);
        return NULL;
    }

    strcpy(newError->method, method);

    return newError;
}

Error* createErrorNoCause(
    ErrorCode code, 
    const char* module,
    const char* method
) {
    return createError(code, module, method, NULL);
}

void exitWithError(Error* error) {
    Error* nextError;

    /* Handle printing the error */
    printErrorWithoutCause(error);
    nextError = error->cause;
    while (nextError != NULL) {
        fprintf(stderr, "caused by ");
        printErrorWithoutCause(nextError);
        nextError = nextError->cause;
    } 

    exit(error->code);
}