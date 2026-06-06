#include "SemanticAnalysis.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include <string.h>
#include <stdlib.h>

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/* PRIVATE FUNCTIONS */

static void _logSemanticAnalysisAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

static SymbolTableEntry * _findSymbol(const char * identifier) {
	SymbolTableEntry * entry = _compilerState->symbolTable;
	while (entry != NULL) {
		if (strcmp(entry->identifier, identifier) == 0) {
			return entry;
		}
		entry = entry->next;
	}
	return NULL;
}

static void _addSymbol(const char * identifier, const char * type) {
	SymbolTableEntry * entry = calloc(1, sizeof(SymbolTableEntry));
	entry->identifier = strdup(identifier);
	entry->type = strdup(type);
	entry->value = NULL;
	entry->next = _compilerState->symbolTable;
	_compilerState->symbolTable = entry;
}

static const char * _declarationTypeName(DeclarationType type) {
	switch (type) {
		case DECLARATION_INCOME:         return "income";
		case DECLARATION_EXPENSES:       return "expenses";
		case DECLARATION_ASSET:          return "asset";
		case DECLARATION_DEBT:           return "debt";
		case DECLARATION_GOAL:           return "goal";
		case DECLARATION_DERIVATED_DATA: return "derivatedData";
		case DECLARATION_DERIVATED_EXPR: return "derivatedExpr";
		case DECLARATION_BALANCE:        return "balance";
		default:                         return "unknown";
	}
}

static void _destroySymbolTable(SymbolTableEntry * table) {
	while (table != NULL) {
		SymbolTableEntry * next = table->next;
		free(table->identifier);
		free(table->type);
		free(table);
		table = next;
	}
}

static void _shutdownSemanticAnalysisModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: SemanticAnalysis...");
		_destroySymbolTable(_compilerState->symbolTable);
		_compilerState->symbolTable = NULL;
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

/* PUBLIC FUNCTIONS */

ModuleDestructor initializeSemanticAnalysisModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("SemanticAnalysis");
	return _shutdownSemanticAnalysisModule;
}

CompilationStatus executeSemanticAnalysis() {
	_logSemanticAnalysisAction(__FUNCTION__);
	Program * program = _compilerState->abstractSyntaxtTree;
	if (program == NULL) {
		logError(_logger, "No AST available for semantic analysis.");
		return FAILED;
	}

	Statement * statement = program->statements;
	while (statement != NULL) {
		if (statement->type == STATEMENT_DECLARATION) {
			Declaration * declaration = statement->declaration;
			if (declaration->name != NULL) {
				if (_findSymbol(declaration->name) != NULL) {
					logError(_logger,
						"Semantic error: identifier \"%s\" is already declared.",
						declaration->name);
					return FAILED;
				}
				_addSymbol(declaration->name, _declarationTypeName(declaration->type));
				logDebugging(_logger, "Added symbol: \"%s\" (%s)",
					declaration->name, _declarationTypeName(declaration->type));
			}
		}
		statement = statement->next;
	}

	logDebugging(_logger, "Semantic analysis completed successfully.");
	return SUCCEEDED;
}
