#ifndef UNEBCC_SEMANTIC_SEMANTIC_H
#define UNEBCC_SEMANTIC_SEMANTIC_H

#include <stdbool.h>
#include "parser/ast.h"

// Returns true if no errors occurred
bool semantic_analysis(ASTNode *ast_main);
// Free all allocated names
void semantic_free_allocated(void);

#endif // UNEBCC_SEMANTIC_SEMANTIC_H
