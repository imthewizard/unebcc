#ifndef UNEBCC_X86_64_EMITTER
#define UNEBCC_X86_64_EMITTER

#include <stdio.h>
#include "program.h"

void x86_64_emit(FILE *file, const x86_64Program *prog);

#endif // UNEBCC_X86_64_EMITTER

