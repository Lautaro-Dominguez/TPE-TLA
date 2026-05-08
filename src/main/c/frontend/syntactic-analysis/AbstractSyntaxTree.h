#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

/**
 * This type definitions allows self-referencing types (e.g., an expression
 * that is made of another expressions, such as talking about you in 3rd
 * person, but without the madness).
 */

typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;
typedef enum ConstantType ConstantType;
typedef enum CurrencyType CurrencyType;
typedef enum PeriodicityType PeriodicityType;
typedef enum PropertyType PropertyType;
typedef enum DeclarationType DeclarationType;
typedef enum QueryType QueryType;
typedef enum CommandType CommandType;
typedef enum StatementType StatementType;

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;
typedef struct Property Property;
typedef struct QueryBlock QueryBlock;
typedef struct Declaration Declaration;
typedef struct Command Command;
typedef struct Statement Statement;
typedef struct Program Program;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */
 
/* Expressions */

enum ExpressionType {
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION
};

enum FactorType {
	CONSTANT,
	EXPRESSION,
	QUERY		
};

/* Constants */

enum ConstantType {
	INTEGER_CONSTANT,
	FLOAT_CONSTANT,
	PERCENTAGE_CONSTANT,
	STRING_CONSTANT,
	DATE_CONSTANT,
	IDENTIFIER_CONSTANT
};

/* Currencies and periodicities */

enum CurrencyType {
	CURRENCY_ARS,
	CURRENCY_USD
};

enum PeriodicityType {
	PERIODICITY_DAILY,
	PERIODICITY_WEEKLY,
	PERIODICITY_BIMONTHLY,
	PERIODICITY_MONTHLY,
	PERIODICITY_YEARLY
};

/* Block properties */

enum PropertyType {
	PROPERTY_VALUE,
	PROPERTY_CURRENCY,
	PROPERTY_PERIODICITY,
	PROPERTY_CATEGORY,
	PROPERTY_FROM,
	PROPERTY_UP,
	PROPERTY_BALANCE,
	PROPERTY_INTEREST,
	PROPERTY_MIN_PAYMENT,
	PROPERTY_AMOUNT,
	PROPERTY_DEADLINE
};

/* Declaration types */

enum DeclarationType {
	DECLARATION_INCOME,
	DECLARATION_EXPENSES,
	DECLARATION_ASSET,
	DECLARATION_DEBT,
	DECLARATION_GOAL,
	DECLARATION_DERIVATED_DATA,
	DECLARATION_DERIVATED_EXPR	
};

/* Aggregate query types */

enum QueryType {
	QUERY_TOTAL_INCOME,
	QUERY_TOTAL_EXPENSES,
	QUERY_MAX_CATEGORY
};

/* Action command types */

enum CommandType {
	COMMAND_EXCHANGE,
	COMMAND_CHANGE_PERIODICITY,
	COMMAND_EXPORT
};

/* Statement type (declaration or command) */

enum StatementType {
	STATEMENT_DECLARATION,
	STATEMENT_COMMAND
};

/* STRUCTS */

struct Constant {
	int value;				
	ConstantType type;
	union {
		int intValue;
		double floatValue;
		char * stringValue;		
	};
};

struct Factor {
	union {
		Constant * constant;
		Expression * expression;
		QueryBlock * query;		
	};
	FactorType type;
};

struct Expression {
	union {
		Factor * factor;
		struct {
			Expression * leftExpression;
			Expression * rightExpression;
		};
	};
	ExpressionType type;
};

/**
 * QueryBlock: bloque "as { from ..., up ..., category = ... }"
 * usado tanto en sentencias de consulta standalone como en expresiones.
 */
struct QueryBlock {
	QueryType type;
	char * from;		
	char * up;	
	char * category;	
};

struct Property {
	PropertyType type;
	union {
		Expression * valueExpr;
		CurrencyType currencyValue;
		PeriodicityType periodicityValue;
		char * stringValue;		
		Expression * upExpr;
		Expression * balanceExpr;
		Expression * interestExpr;
		Expression * minPaymentExpr;
		Expression * amountExpr;
	};
	Property * next;
};

/**
 * Declaration: bloque income/expenses/asset/debt/goal/derivatedData
 * o variable derivada con expresión (derivatedData X = expr;).
 */
struct Declaration {
	DeclarationType type;
	char * name;
	union {
		Property * properties;		      
		struct {
			Expression * derivedExpr;
		};
	};
	Declaration * next;
};

/**
 * Command: exchange / change-periodicity / export / consultas standalone.
 */
struct Command {
	CommandType type;
	char * targetName;		
	union {
		CurrencyType newCurrency;		            
		PeriodicityType newPeriodicity;		
	};
};

/**
 * Statement: unidad de la lista de sentencias del programa.
 * Puede ser una declaración de bloque, una consulta standalone o un comando.
 */
struct Statement {
	StatementType type;
	union {
		Declaration * declaration;
		Command * command;
	};
	Statement * next;
};

/**
 * Program: raíz del AST.
 */
struct Program {
	Expression * expression;	
	Statement * statements;
};

/**
 * Node recursive super-duper-trambolik-destructors.
 */

void destroyConstant(Constant * constant);
void destroyExpression(Expression * expression);
void destroyFactor(Factor * factor);
void destroyQueryBlock(QueryBlock * query);
void destroyProperty(Property * property);
void destroyDeclaration(Declaration * declaration);
void destroyCommand(Command * command);
void destroyStatement(Statement * statement);
void destroyProgram(Program * program);

#endif