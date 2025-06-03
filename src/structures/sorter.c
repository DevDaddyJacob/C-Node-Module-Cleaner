#include <stdlib.h>

#include "node_module.h"
#include "sorter.h"
#include "../error.h"
#include "../utils/common.h"

/*
 * ==================================================
 * Macros
 * ==================================================
 */

#define MIN_RUN 32


/*
 * ==================================================
 * Typedefs & Prototypes
 * ==================================================
 */

typedef int (*Comparator)(void*, void*);

static int compareNodeModuleAsc(void* a, void* b);

static int compareNodeModuleDesc(void* a, void* b);

static void insertionSort(
    void* base,
    int start,
    int end,
    size_t elemSize,
    Comparator compareFunc
);

static void merge(
    void* base,
    int start,
    int mid,
    int end,
    size_t elemSize,
    Comparator compareFunc
);

static void blockSort(
    void* base,
    int count,
    size_t elemSize,
    Comparator compareFunc
);


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

static int compareNodeModuleAsc(void* a, void* b) {
    NodeModule* nodeModuleA = (NodeModule*)a;
    NodeModule* nodeModuleB = (NodeModule*)b;

    return (int)uint64_compare(
        *getNodeModuleSize(nodeModuleA),
        *getNodeModuleSize(nodeModuleB)
    );
}

static int compareNodeModuleDesc(void* a, void* b) {
    NodeModule* nodeModuleA = (NodeModule*)a;
    NodeModule* nodeModuleB = (NodeModule*)b;

    return (int)uint64_compare(
        *getNodeModuleSize(nodeModuleB),
        *getNodeModuleSize(nodeModuleA)
    );
}

static void insertionSort(
    void* base,
    int start,
    int end,
    size_t elemSize,
    Comparator compareFunc
) {
    /* Using char* here is not because we are using that type, but
       rather guaranteed size of 1 byte */
    char* array;
    void* key;
    int i;

    /* Allocate the key memory */
    array = (char*)base;
    key = malloc(elemSize);


    /* INSERT COMMENT HERE */
    for (i = start + 1; i <= end; i++) {
        int j;

        memcpy(key, array + i * elemSize, elemSize);
        j = i - 1;


        /* INSERT COMMENT HERE */
        while (j >= start && compareFunc(array + j * elemSize, key) > 0) {
            memcpy(array + (j + 1) * elemSize, array + j * elemSize, elemSize);
            j--;
        }

        memcpy(array + (j + 1) * elemSize, key, elemSize);
    }


    /* Free used memory */
    free(key);
}

/* Takes 2 sorted sub-arrays and merges them ([start, mid] & [mid+1, end]) */
static void merge(
    void* base,
    int start,
    int mid,
    int end,
    size_t elemSize,
    Comparator compareFunc
) {
    char* array;
    void* leftArray;
    void* rightArray;
    int leftArraySize;
    int rightArraySize;
    int iL;
    int iR;
    int k;
    
    /* Determine the array sizes */
    array = (char*)base;
    leftArraySize = mid - start + 1;
    rightArraySize = end - mid;


    /* Allocate memory for the 2 arrays */
    leftArray = malloc(leftArraySize * elemSize);
    rightArray = malloc(rightArraySize * elemSize);


    /* Use copy to populate the 2 arrays */
    memcpy(leftArray, array + start * elemSize, leftArraySize * elemSize);
    memcpy(rightArray, array + (mid + 1) * elemSize, rightArraySize * elemSize);


    /* INSERT COMMENT HERE */
    iL = 0;
    iR = 0;
    k = start;
    while (iL < leftArraySize && iR < rightArraySize) {
        if (
            compareFunc(
                (char*)leftArray + iL * elemSize,
                (char*)rightArray + iR * elemSize
            ) <= 0
        ) {
            memcpy(
                array + k * elemSize,
                (char*)leftArray + iL * elemSize,
                elemSize
            );
            iL++;
        } else {
            memcpy(
                array + k * elemSize,
                (char*)rightArray + iR * elemSize,
                elemSize
            );
            iR++;
        }

        k++;
    }


    /* INSERT COMMENT HERE */
    while (iL < leftArraySize) {
        memcpy(
            array + k * elemSize,
            (char*)leftArray + iL * elemSize,
            elemSize
        );
        iL++;
        k++;
    }


    /* INSERT COMMENT HERE */
    while (iR < rightArraySize) {
        memcpy(
            array + k * elemSize,
            (char*)rightArray + iR * elemSize,
            elemSize
        );
        iR++;
        k++;
    }


    /* Free the 2 arrays made */
    free(leftArray);
    free(rightArray);
}

static void blockSort(
    void* base,
    int count,
    size_t elemSize,
    Comparator compareFunc
) {
    int i;
    int blockSize;
    
    /* Divide the array into small blocks and apply insertion sort */
    for (i = 0; i < count; i += MIN_RUN) {
        int right;

        right = (i + MIN_RUN - 1) < (count - 1)
            ? (i + MIN_RUN - 1) : (count - 1);

        insertionSort(base, i, right, elemSize, compareFunc);
    }

    
    /* Merge the sorted blocks bottom up */
    for (blockSize = MIN_RUN; blockSize < count; blockSize *= 2) {
        int start;

        for (start = 0; start < count; start += 2 * blockSize) {
            int mid;
            int end;

            mid = start + blockSize - 1;
            end = (start + 2 * blockSize - 1) < (count - 1)
                ? (start + 2 * blockSize - 1) : (count -1);

            if (mid < end) {
                merge(base, start, mid, end, elemSize, compareFunc);
            }
        }
    }
}

void sortNodeModuleList(NodeModuleList* list, SortDirection sortDir) {
    Comparator compareFunc = sortDir == SORT_ASCENDING
        ? compareNodeModuleAsc : compareNodeModuleDesc;

    blockSort(list->data, list->count, sizeof(NodeModule), compareFunc);
}