#include <stdlib.h>

#include "uint64.h"

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

static UInt64 uint64_shiftLeft(UInt64 value, int n);

static UInt64 uint64_setBit(UInt64 value, int bit);

static int uint64_testBit(UInt64 value, int bit);

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

static UInt64 uint64_shiftLeft(UInt64 value, int n) {
    while (n-- > 0) {
        value.high = (value.high << 1) | ((value.low & 0x80000000UL) ? 1 : 0);
        value.low <<= 1;
    }
    
    return value;
}

static UInt64 uint64_setBit(UInt64 value, int bit) {
    if (bit >= 32) {
        value.high |= 1UL << (bit - 32);
    } else {
        value.low |= 1UL << bit;
    }

    return value;
}

static int uint64_testBit(UInt64 value, int bit) {
    if (bit >= 32) {
        return (value.high >> (bit - 32)) & 1UL;
    } else {
        return (value.low >> bit) & 1UL;
    }
}

UInt64Comparison uint64_compare(UInt64 a, UInt64 b) {
    if (a.high < b.high) return UINT64_COMPARE_LESS;
    if (a.high > b.high) return UINT64_COMPARE_GREATER;
    if (a.low < b.low) return UINT64_COMPARE_LESS;
    if (a.low > b.low) return UINT64_COMPARE_GREATER;
    return UINT64_COMPARE_EQUAL;
}

UInt64 uint64_fromUInt32(UInt32 value) {
    UInt64 output;
    output.low = value;
    output.high = 0;

    return output;
}

UInt64 uint64_add(UInt64 a, UInt64 b) {
    UInt64 output;
    output.low = a.low + b.low;
    output.high = a.high + b.high;

    /* Check for an overflow & apply carry */
    if (output.low < a.low) output.high++;

    return output;
}

UInt64 uint64_addUInt32(UInt64 a, UInt32 b) {
    UInt64 output;
    output.low = a.low + b;
    output.high = a.high;

    /* Check for an overflow & apply carry */
    if (output.low < a.low) output.high++;

    return output;
}

UInt64 uint64_subtract(UInt64 a, UInt64 b) {
    UInt64 output;
    output.low = a.low - b.low;
    output.high = a.high - b.high;

    /* Check for an underflow & apply borrow */
    if (output.low > a.low) output.high--;

    return output;
}

UInt64 uint64_subtractUInt32(UInt64 a, UInt32 b) {
    UInt64 output;
    output.low = a.low - b;
    output.high = a.high;

    /* Check for an underflow & apply borrow */
    if (output.low > a.low) output.high--;

    return output;
}

UInt64 uint64_multiply(UInt64 a, UInt64 b) {
    UInt64 output;
    UInt32 mid;
    UInt32 carry;

    UInt32 aLow = a.low;
    UInt32 aHigh = a.high;
    UInt32 bLow = b.low;
    UInt32 bHigh = b.high;


    /* Calculate partial products */
    UInt32 partial_0 = (aLow & 0xFFFF) * (bLow & 0xFFFF);
    UInt32 partial_1 = (aLow >> 16) * (bLow & 0xFFFF);
    UInt32 partial_2 = (aLow & 0xFFFF) * (bLow >> 16);
    UInt32 partial_3 = (aLow >> 16) * (bLow >> 16);


    /* Combine lower 32-bit result */
    carry = 0;
    output.low = (
        partial_0 + 
        ((partial_1 & 0xFFFF) << 16) + 
        ((partial_2 & 0xFFFF) << 16)
    );
    
    if (output.low < partial_0) carry++;


    /* Calculate the high */
    mid = (partial_1 >> 16) + (partial_2 >> 16) + partial_3;
    output.high = aLow * bHigh + aHigh * bLow + mid + carry;

    return output;
}

UInt64 uint64_divide(UInt64 numerator, UInt64 denominator) {
    UInt64 quotient = UINT64_ZERO;
    UInt64 remainder = UINT64_ZERO;
    int i;

    for (i = 63; i >= 0; i--) {
        /* Shift remainder left by 1 and bring in the next bit from numerator */
        remainder = uint64_shiftLeft(remainder, 1);
        if (uint64_testBit(numerator, i)) remainder.low |= 1;


        /* If the remainder >= denominator, subtract and set quotient bit */
        if (UINT64_GREATER_OR_EQUAL(remainder, denominator)) {
            remainder = uint64_subtract(remainder, denominator);
            quotient = uint64_setBit(quotient, i);
        }
    }

    return quotient;
}

UInt64 uint64_divideR(
    UInt64 numerator, 
    UInt64 denominator, 
    UInt64* outRemainder
) {
    UInt64 quotient = UINT64_ZERO;
    UInt64 remainder = UINT64_ZERO;
    int i;

    for (i = 63; i >= 0; i--) {
        /* Shift remainder left by 1 and bring in the next bit from numerator */
        remainder = uint64_shiftLeft(remainder, 1);
        if (uint64_testBit(numerator, i)) remainder.low |= 1;


        /* If the remainder >= denominator, subtract and set quotient bit */
        if (UINT64_GREATER_OR_EQUAL(remainder, denominator)) {
            remainder = uint64_subtract(remainder, denominator);
            quotient = uint64_setBit(quotient, i);
        }
    }

    if (outRemainder != NULL) {
        *outRemainder = remainder;
    }

    return quotient;
}

UInt64 uint64_mod(UInt64 a, UInt64 b) {
    UInt64 remainder = UINT64_ZERO;
    int i;

    for (i = 63; i >= 0; i--) {
        /* Shift remainder left by 1 and bring in the next bit from value (a) */
        remainder = uint64_shiftLeft(remainder, 1);
        if (uint64_testBit(a, i)) remainder.low |= 1;


        /* If the remainder >= factor (b), subtract and set quotient bit */
        if (UINT64_GREATER_OR_EQUAL(remainder, b)) {
            remainder = uint64_subtract(remainder, b);
        }
    }

    return remainder;
}

char* uint64_toStringHex(UInt64 value) {
    static const char* hexChars = "0123456789ABCDEF";
    char* output = malloc(2 + 16 + 1); /* 2 prefix + 16 digit + 1 terminator */
    int i;
    int shift;
    UInt32 nibble;

    output[0] = '0';
    output[1] = 'x';

    /* Extrach each nibble from the high portion, for most to least significant */
    for (i = 0; i < 8; i++) {
        shift = 28 - i * 4;
        nibble = (value.high >> shift) & 0xF;
        output[2 + i] = hexChars[nibble];
    }


    /* Extrach each nibble from the low portion, for most to least significant */
    for (i = 0; i < 8; i++) {
        shift = 28 - i * 4;
        nibble = (value.low >> shift) & 0xF;
        output[2 + i + 8] = hexChars[nibble];
    }

    output[18] = '\0';
    return output;
}

char* uint64_toStringDec(UInt64 value) {
    char* output;
    char* buffer;
    int digits = 0;
    int i;
    UInt64 ten = uint64_fromUInt32(10);
    UInt64 quotient;
    UInt64 remainder;

    /* Handle zero */
    if (UINT64_IS_ZERO(value)) {
        output = (char*)malloc(2 * sizeof(char));
        if (output == NULL) return NULL;
        output[0] = '0';
        output[1] = '\0';

        return output;
    }

    
    /* Allocate the buffer*/
    buffer = (char*)malloc(21 * sizeof(char));
    if (buffer == NULL) return NULL;



    /* Extract digits */
    while (!UINT64_IS_ZERO(value)) {
        quotient = uint64_divideR(value, ten, &remainder);

        buffer[digits++] = (char)('0' + (remainder.low % 10));
        value = quotient;
    }


    /* Reverse the buffer to make the output string */
    output = (char*)malloc((digits + 1) * sizeof(char));
    if (output == NULL) {
        free(buffer);
        return NULL;
    }

    for (i = 0, digits = digits - 1; digits >= 0; digits--, i++) {
        output[i] = buffer[digits];
    }
    output[i] = '\0';

    free(buffer);
    return output;
}

char* uint64_toStringBin(UInt64 value) {
    char* output = malloc(65); /* 2 prefix + 16 digit + 1 terminator */
    int i;

    /* Extrach each nibble from the high portion, for most to least significant */
    for (i = 0; i < 32; i++) {
        output[i] = ((value.high >> (31 - i)) & 1) ? '1' : '0';
    }


    /* Extrach each nibble from the low portion, for most to least significant */
    for (i = 0; i < 32; i++) {
        output[i + 32] = ((value.low >> (31 - i)) & 1) ? '1' : '0';
    }

    output[64] = '\0';
    return output;
}