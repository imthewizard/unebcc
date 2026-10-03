#ifndef UNEBCC_X86_64_GEN_H
#define UNEBCC_X86_64_GEN_H

#include "program.h"
#include "ir/instruction.h"

void x86_64_create_prog(const IRInstruction *instructions, x86_64Program *prog);

#endif // UNEBCC_X86_64_GEN_H
