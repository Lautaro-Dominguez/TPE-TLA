#include "SemanticAnalysis.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/* PRIVATE FUNCTIONS */

static void _logSemanticAnalysisAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* -- Symbol table helpers -- */

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

/* -- Division by zero helpers -- */

/*
 * Attempts to evaluate expr as a fully-constant arithmetic expression.
 * Returns 1 and sets *result on success; returns 0 if any sub-expression
 * contains a non-constant (identifier, query, etc.).
 */
static int _evaluateConstantExpr(Expression * expr, double * result) {
	if (expr == NULL) return 0;
	if (expr->type == FACTOR) {
		Factor * f = expr->factor;
		if (f->type == EXPRESSION) {
			/* Parenthesized sub-expression: recurse into it */
			return _evaluateConstantExpr(f->expression, result);
		}
		if (f->type != CONSTANT) return 0;
		Constant * c = f->constant;
		if (c->type == INTEGER_CONSTANT)    { *result = (double)c->intValue;  return 1; }
		if (c->type == FLOAT_CONSTANT)      { *result = c->floatValue;        return 1; }
		if (c->type == PERCENTAGE_CONSTANT) { *result = c->floatValue;        return 1; }
		return 0;
	}
	double left, right;
	if (!_evaluateConstantExpr(expr->leftExpression,  &left))  return 0;
	if (!_evaluateConstantExpr(expr->rightExpression, &right)) return 0;
	switch (expr->type) {
		case ADDITION:       *result = left + right; return 1;
		case SUBTRACTION:    *result = left - right; return 1;
		case MULTIPLICATION: *result = left * right; return 1;
		case DIVISION:       *result = left / right; return 1;
		default: return 0;
	}
}

static CompilationStatus _checkDivisionByZero(Expression * expr) {
	if (expr == NULL) return SUCCEEDED;
	if (expr->type == DIVISION) {
		double divisor;
		if (_evaluateConstantExpr(expr->rightExpression, &divisor) && divisor == 0.0) {
			logError(_logger, "Semantic error: division by zero detected.");
			return FAILED;
		}
	}
	if (expr->type != FACTOR) {
		if (_checkDivisionByZero(expr->leftExpression) == FAILED)  return FAILED;
		if (_checkDivisionByZero(expr->rightExpression) == FAILED) return FAILED;
	}
	return SUCCEEDED;
}

static CompilationStatus _checkPropertiesDivisionByZero(Property * property) {
	while (property != NULL) {
		CompilationStatus status = SUCCEEDED;
		switch (property->type) {
			case PROPERTY_VALUE:       status = _checkDivisionByZero(property->valueExpr);      break;
			case PROPERTY_UP:          status = _checkDivisionByZero(property->upExpr);         break;
			case PROPERTY_BALANCE:     status = _checkDivisionByZero(property->balanceExpr);    break;
			case PROPERTY_INTEREST:    status = _checkDivisionByZero(property->interestExpr);   break;
			case PROPERTY_MIN_PAYMENT: status = _checkDivisionByZero(property->minPaymentExpr); break;
			case PROPERTY_AMOUNT:      status = _checkDivisionByZero(property->amountExpr);     break;
			default: break;
		}
		if (status == FAILED) return FAILED;
		property = property->next;
	}
	return SUCCEEDED;
}

/* -- Date range helpers -- */

/*
 * Compares two dates in DD-MM-YYYY format.
 * Returns negative if d1 < d2, 0 if equal, positive if d1 > d2.
 * Returns 0 if either string cannot be parsed (safe: no false rejection).
 */
static int _compareDates(const char * d1, const char * d2) {
	int day1, mon1, year1, day2, mon2, year2;
	if (sscanf(d1, "%d-%d-%d", &day1, &mon1, &year1) != 3) return 0;
	if (sscanf(d2, "%d-%d-%d", &day2, &mon2, &year2) != 3) return 0;
	if (year1 != year2) return year1 - year2;
	if (mon1  != mon2)  return mon1  - mon2;
	return day1 - day2;
}

static CompilationStatus _checkDateRange(const char * from, const char * up, const char * context) {
	if (from == NULL || up == NULL) return SUCCEEDED;
	if (_compareDates(from, up) > 0) {
		logError(_logger,
			"Semantic error: 'from' date (%s) is later than 'up' date (%s) in \"%s\".",
			from, up, context);
		return FAILED;
	}
	return SUCCEEDED;
}

/* Extracts the date string from an expression that wraps a DATE_CONSTANT. */
static const char * _extractDateFromExpr(Expression * expr) {
	if (expr == NULL || expr->type != FACTOR) return NULL;
	Factor * f = expr->factor;
	if (f->type != CONSTANT) return NULL;
	if (f->constant->type == DATE_CONSTANT) return f->constant->stringValue;
	return NULL;
}

static CompilationStatus _checkBalanceDateRange(Declaration * decl) {
	const char * from = NULL;
	const char * up   = NULL;
	Property * prop = decl->properties;
	while (prop != NULL) {
		if (prop->type == PROPERTY_FROM) from = prop->stringValue;
		if (prop->type == PROPERTY_UP)   up   = _extractDateFromExpr(prop->upExpr);
		prop = prop->next;
	}
	return _checkDateRange(from, up, decl->name != NULL ? decl->name : "balance");
}

static CompilationStatus _checkQueryBlockDateRange(QueryBlock * query) {
	return _checkDateRange(query->from, query->up, "query block");
}

static CompilationStatus _checkExpressionNumeric(Expression * expr) {
	if (expr == NULL) return SUCCEEDED;

	if (expr->type == FACTOR) {
		Factor * f = expr->factor;
		if (f->type == EXPRESSION) {
			return _checkExpressionNumeric(f->expression);
		}
		if (f->type == QUERY) {
			return SUCCEEDED; /* queries return numeric totals */
		}
		/* f->type == CONSTANT */
		ConstantType ct = f->constant->type;
		if (ct == STRING_CONSTANT || ct == DATE_CONSTANT) {
			logError(_logger,
				"Semantic error: non-numeric value used in arithmetic expression.");
			return FAILED;
		}
		if (ct == IDENTIFIER_CONSTANT) {
			const char * name = f->constant->stringValue;
			SymbolTableEntry * entry = _findSymbol(name);
			if (entry == NULL) {
				logError(_logger,
					"Semantic error: identifier \"%s\" is not declared.", name);
				return FAILED;
			}
			if (strcmp(entry->type, "derivatedData") != 0 &&
			    strcmp(entry->type, "derivatedExpr") != 0) {
				logError(_logger,
					"Semantic error: identifier \"%s\" (type \"%s\") cannot be used in arithmetic expressions.",
					name, entry->type);
				return FAILED;
			}
		}
		return SUCCEEDED;
	}

	if (_checkExpressionNumeric(expr->leftExpression)  == FAILED) return FAILED;
	if (_checkExpressionNumeric(expr->rightExpression) == FAILED) return FAILED;
	return SUCCEEDED;
}

static CompilationStatus _checkPropertiesNumeric(Property * property) {
	while (property != NULL) {
		CompilationStatus status = SUCCEEDED;
		switch (property->type) {
			case PROPERTY_VALUE:       status = _checkExpressionNumeric(property->valueExpr);      break;
			case PROPERTY_BALANCE:     status = _checkExpressionNumeric(property->balanceExpr);    break;
			case PROPERTY_INTEREST:    status = _checkExpressionNumeric(property->interestExpr);   break;
			case PROPERTY_MIN_PAYMENT: status = _checkExpressionNumeric(property->minPaymentExpr); break;
			case PROPERTY_AMOUNT:      status = _checkExpressionNumeric(property->amountExpr);     break;
			/* PROPERTY_UP holds a DATE_CONSTANT (balance declaration) — not a numeric context */
			default: break;
		}
		if (status == FAILED) return FAILED;
		property = property->next;
	}
	return SUCCEEDED;
}

/* -- Command target validation -- */

static CompilationStatus _checkCommandTargetDeclared(Command * cmd) {
	switch (cmd->type) {
		case COMMAND_EXCHANGE:
		case COMMAND_CHANGE_PERIODICITY:
		case COMMAND_EXPORT:
		case COMMAND_CIRCLE_GRAPHIC:
		case COMMAND_CIRCLE_GRAPHIC_BY_CATEGORY:
		case COMMAND_PLAN:
			if (cmd->targetName != NULL && _findSymbol(cmd->targetName) == NULL) {
				logError(_logger,
					"Semantic error: identifier \"%s\" is not declared.",
					cmd->targetName);
				return FAILED;
			}
			break;
		default:
			break;
	}
	return SUCCEEDED;
}

/* -- Module lifecycle -- */

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
			Declaration * decl = statement->declaration;

			/* 1. Duplicate identifier check */
			if (decl->name != NULL) {
				if (_findSymbol(decl->name) != NULL) {
					logError(_logger,
						"Semantic error: identifier \"%s\" is already declared.",
						decl->name);
					return FAILED;
				}
				_addSymbol(decl->name, _declarationTypeName(decl->type));
				logDebugging(_logger, "Added symbol: \"%s\" (%s)",
					decl->name, _declarationTypeName(decl->type));
			}

			/* 2. Division by zero + numeric operand check in property expressions.
			 * DECLARATION_DERIVATED_EXPR uses derivedExpr (union overlap) — skip. */
			if (decl->type != DECLARATION_DERIVATED_EXPR) {
				if (_checkPropertiesDivisionByZero(decl->properties) == FAILED) return FAILED;
				if (_checkPropertiesNumeric(decl->properties)         == FAILED) return FAILED;
			}

			/* 3. Date range in balance declarations */
			if (decl->type == DECLARATION_BALANCE) {
				if (_checkBalanceDateRange(decl) == FAILED) return FAILED;
			}

			/* 4. Checks for derivated expressions */
			if (decl->type == DECLARATION_DERIVATED_EXPR && decl->derivedExpr != NULL) {
				if (_checkDivisionByZero(decl->derivedExpr)    == FAILED) return FAILED;
				if (_checkExpressionNumeric(decl->derivedExpr) == FAILED) return FAILED;
				Expression * expr = decl->derivedExpr;
				if (expr->type == FACTOR && expr->factor->type == QUERY) {
					if (_checkQueryBlockDateRange(expr->factor->query) == FAILED) return FAILED;
				}
			}
		}

		if (statement->type == STATEMENT_COMMAND) {
			Command * cmd = statement->command;

			/* 5. Referenced identifier must already be declared */
			if (_checkCommandTargetDeclared(cmd) == FAILED) return FAILED;

			/* 6. Date range in query blocks — only COMMAND_PROBABILITY has a query field.
			 * Other command types use the same union bytes for currency/periodicity/installments. */
			if (cmd->type == COMMAND_PROBABILITY && cmd->query != NULL) {
				if (_checkQueryBlockDateRange(cmd->query) == FAILED) return FAILED;
			}
		}

		statement = statement->next;
	}

	/* 7. Top-level expression (program->expression mode) */
	if (program->expression != NULL) {
		if (_checkDivisionByZero(program->expression)    == FAILED) return FAILED;
		if (_checkExpressionNumeric(program->expression) == FAILED) return FAILED;
	}

	logDebugging(_logger, "Semantic analysis completed successfully.");
	return SUCCEEDED;
}
