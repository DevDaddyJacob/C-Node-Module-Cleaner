#ifndef h_error
#define h_error

typedef enum {
    ERR_UNKNOWN = 1,
    ERR_APPLICATION,
    ERR_NULL_POINTER,
    ERR_MALLOC_FAILED,
    ERR_WIN_CALL_FAILURE,
    
    ERR_NO_DRIVES_FOUND
} ErrorCode;

typedef struct Error Error;
struct Error {
    ErrorCode code;
    Error* cause;
    char* module;
    char* method;
};

Error* createError(
    ErrorCode code,
    const char* module,
    const char* method,
    Error* cause
);

Error* createErrorNoCause(
    ErrorCode code, 
    const char* module,
    const char* method
);

void exitWithError(Error* error);

#endif /* h_error */