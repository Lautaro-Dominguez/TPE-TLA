#ifndef SEMANTIC_ANALYSIS_HEADER
#define SEMANTIC_ANALYSIS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"

/** Initialize module's internal state. */
ModuleDestructor initializeSemanticAnalysisModule(CompilerState * compilerState);

/**
 * Traverses the AST, builds the symbol table and validates that no two
 * declarations share the same identifier.  Returns SUCCEEDED or FAILED.
 */
CompilationStatus executeSemanticAnalysis();

#endif
