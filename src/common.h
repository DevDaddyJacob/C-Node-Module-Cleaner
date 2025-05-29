#ifndef h_common
#define h_common

#include <string.h>


#define MAX_PATH_LENGTH 260


#define STR_EQUALS(strA, strB) \
    (strcmp(strA, strB) == 0)

    
#define STR_ENDS_WITH_CHAR(str, end) \
    ((str[strlen(str) - 1] == end))

#endif /* h_common */