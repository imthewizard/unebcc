#ifndef UNEBCC_DRIVER_DRIVER_H
#define UNEBCC_DRIVER_DRIVER_H

#include "args.h"

// Attempts to start the compilation process: lexer, parser, ir and assembler
void driver_start(const ArgsContext *ctx);

#endif // UNEBCC_DRIVER_DRIVER_H
