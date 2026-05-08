#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

/* Constants */

Constant * IntegerConstantSemanticAction(const int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant * constant = calloc(1, sizeof(Constant));
	constant->type = INTEGER_CONSTANT;
	constant->value = value;
	constant->intValue = value;
	return constant;
}

Constant * FloatConstantSemanticAction(const double value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant * constant = calloc(1, sizeof(Constant));
	constant->type = FLOAT_CONSTANT;
	constant->floatValue = value;
	return constant;
}

Constant * PercentageConstantSemanticAction(const double value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant * constant = calloc(1, sizeof(Constant));
	constant->type = PERCENTAGE_CONSTANT;
	constant->floatValue = value;
	return constant;
}

Constant * StringConstantSemanticAction(char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant * constant = calloc(1, sizeof(Constant));
	constant->type = STRING_CONSTANT;
	constant->stringValue = value;
	return constant;
}

Constant * DateConstantSemanticAction(char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant * constant = calloc(1, sizeof(Constant));
	constant->type = DATE_CONSTANT;
	constant->stringValue = value;
	return constant;
}

Constant * IdentifierConstantSemanticAction(char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant * constant = calloc(1, sizeof(Constant));
	constant->type = IDENTIFIER_CONSTANT;
	constant->stringValue = value;
	return constant;
}

/* Factors */

Factor * ConstantFactorSemanticAction(Constant * constant) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->constant = constant;
	factor->type = CONSTANT;
	return factor;
}

Factor * ExpressionFactorSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->expression = expression;
	factor->type = EXPRESSION;
	return factor;
}

Factor * QueryFactorSemanticAction(QueryBlock * query) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->query = query;
	factor->type = QUERY;
	return factor;
}

/* Expressions */

Expression * FactorExpressionSemanticAction(Factor * factor) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->factor = factor;
	expression->type = FACTOR;
	return expression;
}

Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

/* QueryBlock */

QueryBlock * TotalIncomeQuerySemanticAction(char * from, char * up) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	QueryBlock * query = calloc(1, sizeof(QueryBlock));
	query->type = QUERY_TOTAL_INCOME;
	query->from = from;
	query->up = up;
	query->category = NULL;
	return query;
}

QueryBlock * TotalExpensesQuerySemanticAction(char * from, char * up, char * category) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	QueryBlock * query = calloc(1, sizeof(QueryBlock));
	query->type = QUERY_TOTAL_EXPENSES;
	query->from = from;
	query->up = up;
	query->category = category;
	return query;
}

QueryBlock * MaxCategoryQuerySemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	QueryBlock * query = calloc(1, sizeof(QueryBlock));
	query->type = QUERY_MAX_CATEGORY;
	query->from = NULL;
	query->up = NULL;
	query->category = NULL;
	return query;
}

/* Properties */

Property * ValuePropertySemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Property * property = calloc(1, sizeof(Property));
	property->type = PROPERTY_VALUE;
	property->valueExpr = expression;
	property->next = NULL;
	return property;
}

Property * CurrencyPropertySemanticAction(CurrencyType currency) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Property * property = calloc(1, sizeof(Property));
	property->type = PROPERTY_CURRENCY;
	property->currencyValue = currency;
	property->next = NULL;
	return property;
}

Property * PeriodicityPropertySemanticAction(PeriodicityType periodicity) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Property * property = calloc(1, sizeof(Property));
	property->type = PROPERTY_PERIODICITY;
	property->periodicityValue = periodicity;
	property->next = NULL;
	return property;
}

Property * CategoryPropertySemanticAction(char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Property * property = calloc(1, sizeof(Property));
	property->type = PROPERTY_CATEGORY;
	property->stringValue = value;
	property->next = NULL;
	return property;
}

Property * FromPropertySemanticAction(char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Property * property = calloc(1, sizeof(Property));
	property->type = PROPERTY_FROM;
	property->stringValue = value;
	property->next = NULL;
	return property;
}

Property * UpPropertySemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Property * property = calloc(1, sizeof(Property));
	property->type = PROPERTY_UP;
	property->upExpr = expression;
	property->next = NULL;
	return property;
}

Property * BalancePropertySemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Property * property = calloc(1, sizeof(Property));
	property->type = PROPERTY_BALANCE;
	property->balanceExpr = expression;
	property->next = NULL;
	return property;
}

Property * InterestPropertySemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Property * property = calloc(1, sizeof(Property));
	property->type = PROPERTY_INTEREST;
	property->interestExpr = expression;
	property->next = NULL;
	return property;
}

Property * MinPaymentPropertySemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Property * property = calloc(1, sizeof(Property));
	property->type = PROPERTY_MIN_PAYMENT;
	property->minPaymentExpr = expression;
	property->next = NULL;
	return property;
}

Property * AmountPropertySemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Property * property = calloc(1, sizeof(Property));
	property->type = PROPERTY_AMOUNT;
	property->amountExpr = expression;
	property->next = NULL;
	return property;
}

Property * DeadlinePropertySemanticAction(char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Property * property = calloc(1, sizeof(Property));
	property->type = PROPERTY_DEADLINE;
	property->stringValue = value;
	property->next = NULL;
	return property;
}

Property * AppendPropertySemanticAction(Property * list, Property * property) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (list == NULL) {
		return property;
	}
	Property * tail = list;
	while (tail->next != NULL) {
		tail = tail->next;
	}
	tail->next = property;
	return list;
}

/* Declarations */

static Declaration * _makeDeclaration(DeclarationType type, char * name, Property * properties) {
	Declaration * declaration = calloc(1, sizeof(Declaration));
	declaration->type = type;
	declaration->name = name;
	declaration->properties = properties;
	declaration->next = NULL;
	return declaration;
}

Declaration * IncomeDeclarationSemanticAction(char * name, Property * properties) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return _makeDeclaration(DECLARATION_INCOME, name, properties);
}

Declaration * ExpensesDeclarationSemanticAction(char * name, Property * properties) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return _makeDeclaration(DECLARATION_EXPENSES, name, properties);
}

Declaration * AssetDeclarationSemanticAction(char * name, Property * properties) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return _makeDeclaration(DECLARATION_ASSET, name, properties);
}

Declaration * DebtDeclarationSemanticAction(char * name, Property * properties) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return _makeDeclaration(DECLARATION_DEBT, name, properties);
}

Declaration * GoalDeclarationSemanticAction(char * name, Property * properties) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return _makeDeclaration(DECLARATION_GOAL, name, properties);
}

Declaration * DerivatedDataDeclarationSemanticAction(char * name, Property * properties) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return _makeDeclaration(DECLARATION_DERIVATED_DATA, name, properties);
}

Declaration * DerivatedExprDeclarationSemanticAction(char * name, Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Declaration * declaration = calloc(1, sizeof(Declaration));
	declaration->type = DECLARATION_DERIVATED_EXPR;
	declaration->name = name;
	declaration->derivedExpr = expression;
	declaration->next = NULL;
	return declaration;
}

/* Commands */

Command * ExchangeCommandSemanticAction(char * name, CurrencyType currency) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Command * command = calloc(1, sizeof(Command));
	command->type = COMMAND_EXCHANGE;
	command->targetName = name;
	command->newCurrency = currency;
	return command;
}

Command * ChangePeriodicityCommandSemanticAction(char * name, PeriodicityType periodicity) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Command * command = calloc(1, sizeof(Command));
	command->type = COMMAND_CHANGE_PERIODICITY;
	command->targetName = name;
	command->newPeriodicity = periodicity;
	return command;
}

Command * ExportCommandSemanticAction(char * name) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Command * command = calloc(1, sizeof(Command));
	command->type = COMMAND_EXPORT;
	command->targetName = name;
	return command;
}

/* Statements */

Statement * DeclarationStatementSemanticAction(Declaration * declaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = calloc(1, sizeof(Statement));
	statement->type = STATEMENT_DECLARATION;
	statement->declaration = declaration;
	statement->next = NULL;
	return statement;
}

Statement * CommandStatementSemanticAction(Command * command) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Statement * statement = calloc(1, sizeof(Statement));
	statement->type = STATEMENT_COMMAND;
	statement->command = command;
	statement->next = NULL;
	return statement;
}

Statement * QueryStatementSemanticAction(QueryBlock * query) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Command * command = calloc(1, sizeof(Command));
	command->type = (query->type == QUERY_MAX_CATEGORY)
		? COMMAND_EXPORT
		: COMMAND_EXPORT;
	command->targetName = NULL;
	free(command);

	Factor * factor = QueryFactorSemanticAction(query);
	Expression * expression = FactorExpressionSemanticAction(factor);
	Declaration * declaration = DerivatedExprDeclarationSemanticAction(NULL, expression);

	Statement * statement = calloc(1, sizeof(Statement));
	statement->type = STATEMENT_DECLARATION;
	statement->declaration = declaration;
	statement->next = NULL;
	return statement;
}

Statement * AppendStatementSemanticAction(Statement * list, Statement * statement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (list == NULL) {
		return statement;
	}
	Statement * tail = list;
	while (tail->next != NULL) {
		tail = tail->next;
	}
	tail->next = statement;
	return list;
}

/* Program */

Program * ExpressionProgramSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->expression = expression;
	program->statements = NULL;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

Program * StatementListProgramSemanticAction(Statement * statements) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->expression = NULL;
	program->statements = statements;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}