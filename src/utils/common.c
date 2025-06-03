#include <conio.h>
#include <stdio.h>
#include <stdarg.h>

#include "common.h"

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

void reprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    printf("\r");
    vprintf(format, args);
    fflush(stdout);

    va_end(args);
}

void moveUpLine() {
    printf("\033[1A");
}

SelectionKey awaitNextSelectionKey() {
    for (;;) {
        switch (_getch()) {
            case 27: return SELECT_KEY_ESCAPE;
            case 13: return SELECT_KEY_ENTER;
            case 32: return SELECT_KEY_SPACE;

            case 0:
            case 224: {
                switch (_getch()) {
                    case 72: return SELECT_KEY_UP;
                    case 75: return SELECT_KEY_LEFT;
                    case 77: return SELECT_KEY_RIGHT;
                    case 80: return SELECT_KEY_DOWN;
                }
            }
        }
    }
}