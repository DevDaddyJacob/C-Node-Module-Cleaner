#ifndef h_common
#define h_common

#include <string.h>


#define MAX_PATH_LENGTH 260


#define STR_EQUALS(strA, strB) \
    (strcmp(strA, strB) == 0)

    
#define STR_ENDS_WITH_CHAR(str, end) \
    ((str[strlen(str) - 1] == end))

    
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


void reprintf(const char* format, ...);

void moveUpLine();


typedef enum {
    SELECT_KEY_UP,
    SELECT_KEY_RIGHT,
    SELECT_KEY_DOWN,
    SELECT_KEY_LEFT,
    SELECT_KEY_SPACE,
    SELECT_KEY_ENTER,
    SELECT_KEY_ESCAPE,
} SelectionKey;

SelectionKey awaitNextSelectionKey();

#endif /* h_common */