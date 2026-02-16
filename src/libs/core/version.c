#include "version.h"

#include <stdio.h>

void printVersion()
{
    fputs("LCT version " VERSION "\n", stdout); 
    fputs("Compiled on " __DATE__ "\n", stdout);
    fputs("License: BSD 3-Clause\n", stdout);
    fflush(stdout);
}
