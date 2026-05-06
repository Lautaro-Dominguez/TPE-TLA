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

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;
typedef struct Property Property;
typedef struct Declaration Declaration;
typedef struct Program Program;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum ExpressionType {
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION
};

enum FactorType {
	CONSTANT,
	EXPRESSION
};

enum ConstantType {
	INTEGER_CONSTANT,
	FLOAT_CONSTANT,
	PERCENTAGE_CONSTANT,
	STRING_CONSTANT,
	DATE_CONSTANT,
	IDENTIFIER_CONSTANT
};

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

enum DeclarationType {
	DECLARATION_INCOME,
	DECLARATION_EXPENSES,
	DECLARATION_ASSET,
	DECLARATION_DEBT,
	DECLARATION_GOAL,
	DECLARATION_DERIVATED_DATA
};

/**
 * Constant keeps the original "value" field so Generator.c compiles
 * unchanged, and adds "type" plus a union for the richer literal types
 * introduced by the new language.
 */
struct Constant {
	int value;				/* kept for Generator.c compatibility  */
	ConstantType type;
	union {
		int intValue;
		double floatValue;
		char * stringValue;		/* STRING, DATE, IDENTIFIER */
	};
};

struct Factor {
	union {
		Constant * constant;
		Expression * expression;
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

struct Property {
	PropertyType type;
	union {
		Expression * valueExpr;			/* value       */
		CurrencyType currencyValue;		/* currency    */
		PeriodicityType periodicityValue;	/* periodicity */
		char * stringValue;			/* category, from, deadline */
		Expression * upExpr;			/* up          */
		Expression * balanceExpr;		/* balance     */
		Expression * interestExpr;		/* interest    */
		Expression * minPaymentExpr;		/* minPayment  */
		Expression * amountExpr;		/* amount      */
	};
	Property * next;
};

struct Declaration {
	DeclarationType type;
	char * name;
	Property * properties;
	Declaration * next;
};

/**
 * Program keeps the original "expression" field so Generator.c compiles
 * unchanged, and adds "declarations" for the new language structure.
 * For valid programs, "declarations" is used; "expression" is NULL.
 */
struct Program {
	Expression * expression;		/* kept for Generator.c compatibility */
	Declaration * declarations;
};

/**
 * Node recursive super-duper-trambolik-destructors.
 */

void destroyConstant(Constant * constant);
void destroyExpression(Expression * expression);
void destroyFactor(Factor * factor);
void destroyProperty(Property * property);
void destroyDeclaration(Declaration * declaration);
void destroyProgram(Program * program);

#endif