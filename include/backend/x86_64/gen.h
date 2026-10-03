#ifndef UNEBCC_X86_64_GEN_H
#define UNEBCC_X86_64_GEN_H

#include "program.h"
#include "ir/function.h"

void x86_64_create_prog(const IRFunction *functions, x86_64Program *prog);

#endif // UNEBCC_X86_64_GEN_H
