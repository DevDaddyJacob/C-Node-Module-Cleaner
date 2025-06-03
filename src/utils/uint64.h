#ifndef h_uint64
#define h_uint64

#ifndef UInt32
typedef unsigned long UInt32;
#endif

typedef enum {
    UINT64_COMPARE_LESS = -1,
    UINT64_COMPARE_EQUAL = 0,
    UINT64_COMPARE_GREATER = 1
} UInt64Comparison;

typedef struct {
    UInt32 low;      /* Least significant 32 bits */
    UInt32 high;     /* Most significant 32 bits */
} UInt64;

#define UINT64_MAX \
    ((UInt64){ 0xFFFFFFFF, 0xFFFFFFFF })

#define UINT64_MIN \
    ((UInt64){ 0, 0 })

#define UINT64_ZERO \
    UINT64_MIN

#define UINT64_IS_ZERO(value) \
    (value.high == 0 && value.low == 0)

#define UINT64_GREATER(a, b) \
    (uint64_compare(a, b) == UINT64_COMPARE_GREATER)

#define UINT64_LESS(a, b) \
    (uint64_compare(a, b) == UINT64_COMPARE_LESS)

#define UINT64_EQUAL(a, b) \
    (uint64_compare(a, b) == UINT64_COMPARE_EQUAL)

#define UINT64_NOT_EQUAL(a, b) \
    (!UINT64_EQUAL(a, b))

#define UINT64_GREATER_OR_EQUAL(a, b) \
    (uint64_compare(a, b) != UINT64_COMPARE_LESS)

#define UINT64_LESS_OR_EQUAL(a, b) \
    (uint64_compare(a, b) != UINT64_COMPARE_GREATER)

UInt64Comparison uint64_compare(UInt64 a, UInt64 b);

UInt64 uint64_fromUInt32(UInt32 value);

UInt64 uint64_add(UInt64 a, UInt64 b);

UInt64 uint64_addUInt32(UInt64 a, UInt32 b);

UInt64 uint64_subtract(UInt64 a, UInt64 b);

UInt64 uint64_subtractUInt32(UInt64 a, UInt32 b);

UInt64 uint64_multiply(UInt64 a, UInt64 b);

UInt64 uint64_divide(UInt64 numerator, UInt64 denominator);

UInt64 uint64_divideR(
    UInt64 numerator, 
    UInt64 denominator, 
    UInt64* outRemainder
);

UInt64 uint64_mod(UInt64 a, UInt64 b);

char* uint64_toStringHex(UInt64 value);

char* uint64_toStringDec(UInt64 value);

char* uint64_toStringBin(UInt64 value);

#endif /* h_uint64 */