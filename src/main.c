#include <stdlib.h>

#include "app.h"
#include "error.h"

int main() {
    Error* runAppError = runApplication();
    if (runAppError != NULL) {
        exitWithError(runAppError);
    }

    return 0;
}