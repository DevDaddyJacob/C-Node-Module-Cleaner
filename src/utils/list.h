#ifndef h_list
#define h_list

#include <stdlib.h>


#define LIST_DEF_STRUCT(dataType) \
    typedef struct { \
        int count; \
        int capacity; \
        dataType* data; \
    }



#define LIST_FUNC_NAME_ALLOC(listType) \
    alloc##listType

#define LIST_FUNC_SIGNATURE_ALLOC(listType) \
    listType* LIST_FUNC_NAME_ALLOC(listType)()

#define LIST_FUNC_BODY_ALLOC(listType) \
    do { \
        listType* list; \
        \
        /* Allocate memory for the list */ \
        list = (listType*)malloc(sizeof(listType)); \
        if (list == NULL) { return NULL; } \
        \
        \
        /* Reset the list to a zero'ed state */ \
        LIST_FUNC_NAME_RESET(listType)(list); \
        \
        return list; \
    } while (0);

#define LIST_FUNCTION_ALLOC(listType) \
    LIST_FUNC_SIGNATURE_ALLOC(listType) { \
        LIST_FUNC_BODY_ALLOC(listType) \
    }



#define LIST_FUNC_NAME_FREE(listType) \
    free##listType

#define LIST_FUNC_SIGNATURE_FREE(listType) \
    void LIST_FUNC_NAME_FREE(listType)(listType* list)

#define LIST_FUNC_BODY_FREE(listType) \
    do { \
        /* Free the list's chunk of memory */ \
        free(list->data); \
        \
        \
        /* Reset the list */ \
        LIST_FUNC_NAME_RESET(listType); \
        \
        \
        /* Free the list pointer */ \
        free(list);\
    } while (0);

#define LIST_FUNCTION_FREE(listType) \
    LIST_FUNC_SIGNATURE_FREE(listType) { \
        LIST_FUNC_BODY_FREE(listType) \
    }



#define LIST_FUNC_NAME_RESET(listType) \
    reset##listType

#define LIST_FUNC_SIGNATURE_RESET(listType) \
    void LIST_FUNC_NAME_RESET(listType)(listType* list)

#define LIST_FUNC_BODY_RESET(listType) \
    do { \
        list->count = 0; \
        list->capacity = 0; \
        list->data = NULL; \
    } while (0);

#define LIST_FUNCTION_RESET(listType) \
    LIST_FUNC_SIGNATURE_RESET(listType) { \
        LIST_FUNC_BODY_RESET(listType) \
    }



#define LIST_FUNC_NAME_WRITE(listType) \
    write##listType

#define LIST_FUNC_SIGNATURE_WRITE(listType, dataType) \
    void LIST_FUNC_NAME_WRITE(listType)(listType* list, dataType value)

#define LIST_FUNC_BODY_WRITE(listType, dataType) \
    do { \
        /* Check if the list needs to be expanded */ \
        if (list->capacity < list->count + 1) { \
            list->capacity = (list->capacity) < 1 ? 1 : list->capacity * 2; \
            list->data = realloc( \
                list->data, \
                sizeof(dataType) * list->capacity \
            ); \
        } \
        \
        \
        /* Add the data to the end of the array */ \
        list->data[list->count] = value; \
        list->count++; \
    } while (0);

#define LIST_FUNCTION_WRITE(listType, dataType) \
    LIST_FUNC_SIGNATURE_WRITE(listType, dataType) { \
        LIST_FUNC_BODY_WRITE(listType, dataType) \
    }



#define LIST_FUNC_NAME_READ(listType) \
    read##listType

#define LIST_FUNC_SIGNATURE_READ(listType, dataType) \
    dataType* LIST_FUNC_NAME_READ(listType)(listType* list, int index)

#define LIST_FUNC_BODY_READ(listType) \
    do { \
        /* Check if out of bounds */ \
        if (index > list->count - 1 || index < 0) { return NULL; } \
        \
        return list->data + index; \
    } while (0);

#define LIST_FUNCTION_READ(listType, dataType) \
    LIST_FUNC_SIGNATURE_READ(listType, dataType) { \
        LIST_FUNC_BODY_READ(listType) \
    }

#endif /* h_list */