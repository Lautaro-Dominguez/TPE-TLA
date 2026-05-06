#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState);

/**
 * Bison semantic actions.
 */

/* ── Constantes ─────────────────────────────────────────────────────────── */

Constant * IntegerConstantSemanticAction(const int value);
Constant * FloatConstantSemanticAction(const double value);
Constant * PercentageConstantSemanticAction(const double value);
Constant * StringConstantSemanticAction(char * value);
Constant * DateConstantSemanticAction(char * value);
Constant * IdentifierConstantSemanticAction(char * value);

/* ── Factores ───────────────────────────────────────────────────────────── */

Factor * ConstantFactorSemanticAction(Constant * constant);
Factor * ExpressionFactorSemanticAction(Expression * expression);

/* ── Expresiones ────────────────────────────────────────────────────────── */

Expression * FactorExpressionSemanticAction(Factor * factor);
Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type);

/* ── Propiedades ────────────────────────────────────────────────────────── */

Property * ValuePropertySemanticAction(Expression * expression);
Property * CurrencyPropertySemanticAction(CurrencyType currency);
Property * PeriodicityPropertySemanticAction(PeriodicityType periodicity);
Property * CategoryPropertySemanticAction(char * value);
Property * FromPropertySemanticAction(char * value);
Property * UpPropertySemanticAction(Expression * expression);
Property * BalancePropertySemanticAction(Expression * expression);
Property * InterestPropertySemanticAction(Expression * expression);
Property * MinPaymentPropertySemanticAction(Expression * expression);
Property * AmountPropertySemanticAction(Expression * expression);
Property * DeadlinePropertySemanticAction(char * value);
Property * AppendPropertySemanticAction(Property * list, Property * property);

/* ── Declaraciones ──────────────────────────────────────────────────────── */

Declaration * IncomeDeclarationSemanticAction(char * name, Property * properties);
Declaration * ExpensesDeclarationSemanticAction(char * name, Property * properties);
Declaration * AssetDeclarationSemanticAction(char * name, Property * properties);
Declaration * DebtDeclarationSemanticAction(char * name, Property * properties);
Declaration * GoalDeclarationSemanticAction(char * name, Property * properties);
Declaration * DerivatedDataDeclarationSemanticAction(char * name, Property * properties);
Declaration * AppendDeclarationSemanticAction(Declaration * list, Declaration * declaration);

/* ── Programa ───────────────────────────────────────────────────────────── */

Program * ExpressionProgramSemanticAction(Expression * expression);
Program * DeclarationListProgramSemanticAction(Declaration * declarations);

#endif