#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntaxTree");
	return _shutdownAbstractSyntaxTreeModule;
}

/* PUBLIC FUNCTIONS */

void destroyConstant(Constant * constant) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (constant != NULL) {
		if (constant->type == STRING_CONSTANT
				|| constant->type == DATE_CONSTANT
				|| constant->type == IDENTIFIER_CONSTANT) {
			free(constant->stringValue);
		}
		free(constant);
	}
}

void destroyExpression(Expression * expression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (expression != NULL) {
		switch (expression->type) {
			case ADDITION:
			case DIVISION:
			case MULTIPLICATION:
			case SUBTRACTION:
				destroyExpression(expression->leftExpression);
				destroyExpression(expression->rightExpression);
				break;
			case FACTOR:
				destroyFactor(expression->factor);
				break;
		}
		free(expression);
	}
}

void destroyFactor(Factor * factor) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (factor != NULL) {
		switch (factor->type) {
			case CONSTANT:
				destroyConstant(factor->constant);
				break;
			case EXPRESSION:
				destroyExpression(factor->expression);
				break;
		}
		free(factor);
	}
}

void destroyProperty(Property * property) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (property != NULL) {
		destroyProperty(property->next);
		switch (property->type) {
			case PROPERTY_VALUE:
				destroyExpression(property->valueExpr);
				break;
			case PROPERTY_UP:
				destroyExpression(property->upExpr);
				break;
			case PROPERTY_BALANCE:
				destroyExpression(property->balanceExpr);
				break;
			case PROPERTY_INTEREST:
				destroyExpression(property->interestExpr);
				break;
			case PROPERTY_MIN_PAYMENT:
				destroyExpression(property->minPaymentExpr);
				break;
			case PROPERTY_AMOUNT:
				destroyExpression(property->amountExpr);
				break;
			case PROPERTY_CATEGORY:
			case PROPERTY_FROM:
			case PROPERTY_DEADLINE:
				free(property->stringValue);
				break;
			case PROPERTY_CURRENCY:
			case PROPERTY_PERIODICITY:
				/* Enumerados: no hay heap memory que liberar */
				break;
		}
		free(property);
	}
}

void destroyDeclaration(Declaration * declaration) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (declaration != NULL) {
		destroyDeclaration(declaration->next);
		free(declaration->name);
		destroyProperty(declaration->properties);
		free(declaration);
	}
}

void destroyProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		destroyExpression(program->expression);
		destroyDeclaration(program->declarations);
		free(program);
	}
}