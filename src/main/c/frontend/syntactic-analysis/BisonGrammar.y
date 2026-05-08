%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

/**
 * The error reporting function for Bison parser.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
 */
void yyerror(const YYLTYPE * location, const char * message) {}

%}

// You touch this, and you die.
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
	/** Terminals. */
	signed int integer;
	double decimal;
	char * string;
	TokenLabel token;

	/** Non-terminals. */
	Constant * constant;
	Expression * expression;
	Factor * factor;
	Program * program;
	Property * property;
	Declaration * declaration;
	Command * command;
	Statement * statement;
	QueryBlock * queryBlock;

	CurrencyType currencyType;
	PeriodicityType periodicityType;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { destroyConstant($$); } <constant>
%destructor { destroyExpression($$); } <expression>
%destructor { destroyFactor($$); } <factor>
%destructor { destroyProperty($$); } <property>
%destructor { destroyDeclaration($$); } <declaration>
%destructor { destroyCommand($$); } <command>
%destructor { destroyStatement($$); } <statement>
%destructor { destroyQueryBlock($$); } <queryBlock>
%destructor { free($$); } <string>

/** Terminals. */
%token <integer> INTEGER
%token <decimal> FLOAT
%token <decimal> PERCENTAGE
%token <string>  STRING
%token <string>  DATE
%token <string>  IDENTIFIER

%token <token> ADD
%token <token> SUB
%token <token> MUL
%token <token> DIV

%token <token> OPEN_PARENTHESIS
%token <token> CLOSE_PARENTHESIS
%token <token> OPEN_BRACE
%token <token> CLOSE_BRACE
%token <token> OPEN_COMMENT
%token <token> CLOSE_COMMENT

%token <token> LT
%token <token> GT
%token <token> EQ
%token <token> NE
%token <token> LE
%token <token> GE

%token <token> COMMA
%token <token> SEMICOLON

/* Tipos de bloque */
%token <token> INCOME
%token <token> EXPENSES
%token <token> ASSET
%token <token> DEBT
%token <token> GOAL
%token <token> DERIVATED_DATA

/* Consultas agregadas */
%token <token> TOTAL_INCOME
%token <token> TOTAL_EXPENSES
%token <token> MAX_CATEGORY

/* Comandos de acción */
%token <token> EXCHANGE
%token <token> CHANGE_PERIODICITY
%token <token> EXPORT

/* Propiedades */
%token <token> AS
%token <token> VALUE
%token <token> CURRENCY
%token <token> PERIODICITY
%token <token> CATEGORY
%token <token> FROM
%token <token> UP
%token <token> BALANCE
%token <token> INTEREST
%token <token> MIN_PAYMENT
%token <token> AMOUNT
%token <token> DEADLINE

/* Periodicidades */
%token <token> MONTHLY
%token <token> WEEKLY
%token <token> DAILY
%token <token> YEARLY
%token <token> BIMONTHLY

/* Monedas */
%token <token> ARS
%token <token> USD

%token <token> IGNORED
%token <token> UNKNOWN

/** Non-terminals. */
%type <program>         program
%type <statement>       statementList statement
%type <declaration>     declaration
%type <command>         command
%type <queryBlock>      queryBlock totalIncomeQuery totalExpensesQuery
%type <property>        propertyList property
%type <string>          queryFrom queryUp queryCategory
%type <currencyType>    currencyValue
%type <periodicityType> periodicityValue
%type <expression>      expression
%type <factor>          factor
%type <constant>        constant

/**
 * Precedence and associativity.
 *
 * @see https://en.cppreference.com/w/cpp/language/operator_precedence.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left ADD SUB
%left MUL DIV

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

/* PROGRAMA — símbolo inicial */

program: statementList								{ $$ = StatementListProgramSemanticAction($1); }
	| expression									{ $$ = ExpressionProgramSemanticAction($1); }
	;

/* LISTA DE SENTENCIAS */

statementList: statement							{ $$ = $1; }
	| statementList statement						{ $$ = AppendStatementSemanticAction($1, $2); }
	;

/* SENTENCIA
 * Puede ser una declaración, un comando o una consulta standalone. 
 */

statement: declaration								{ $$ = DeclarationStatementSemanticAction($1); }
	| command										{ $$ = CommandStatementSemanticAction($1); }
	| queryBlock SEMICOLON							{ $$ = QueryStatementSemanticAction($1); }
	;

/* DECLARACIONES DE BLOQUE
 *
 * Sintaxis:  <keyword> <IDENTIFIER> as { <propertyList> };
 *            derivatedData <IDENTIFIER> = <expression>;
 */

declaration: INCOME IDENTIFIER AS OPEN_BRACE propertyList CLOSE_BRACE SEMICOLON
		{ $$ = IncomeDeclarationSemanticAction($2, $5); }
	| EXPENSES IDENTIFIER AS OPEN_BRACE propertyList CLOSE_BRACE SEMICOLON
		{ $$ = ExpensesDeclarationSemanticAction($2, $5); }
	| ASSET IDENTIFIER AS OPEN_BRACE propertyList CLOSE_BRACE SEMICOLON
		{ $$ = AssetDeclarationSemanticAction($2, $5); }
	| DEBT IDENTIFIER AS OPEN_BRACE propertyList CLOSE_BRACE SEMICOLON
		{ $$ = DebtDeclarationSemanticAction($2, $5); }
	| GOAL IDENTIFIER AS OPEN_BRACE propertyList CLOSE_BRACE SEMICOLON
		{ $$ = GoalDeclarationSemanticAction($2, $5); }
	| DERIVATED_DATA IDENTIFIER AS OPEN_BRACE propertyList CLOSE_BRACE SEMICOLON
		{ $$ = DerivatedDataDeclarationSemanticAction($2, $5); }
	| DERIVATED_DATA IDENTIFIER EQ expression SEMICOLON
		{ $$ = DerivatedExprDeclarationSemanticAction($2, $4); }
	;

/* COMANDOS DE ACCIÓN
 *
 * exchange <IDENTIFIER> <currency>;
 * change-periodicity <IDENTIFIER> <periodicity>;
 * export <IDENTIFIER>;
 */

command: EXCHANGE IDENTIFIER currencyValue SEMICOLON
		{ $$ = ExchangeCommandSemanticAction($2, $3); }
	| CHANGE_PERIODICITY IDENTIFIER periodicityValue SEMICOLON
		{ $$ = ChangePeriodicityCommandSemanticAction($2, $3); }
	| EXPORT IDENTIFIER SEMICOLON
		{ $$ = ExportCommandSemanticAction($2); }
	;

/* CONSULTAS AGREGADAS  (pueden ser standalone o dentro de expresiones)
 *
 * totalIncome as { from <date>, up <date> }
 * totalExpenses as { from <date>, up <date> }
 * totalExpenses as { from <date>, up <date>, category = <string> }
 * maxCategory
 */

queryBlock: totalIncomeQuery						{ $$ = $1; }
	| totalExpensesQuery							{ $$ = $1; }
	| MAX_CATEGORY									{ $$ = MaxCategoryQuerySemanticAction(); }
	;

totalIncomeQuery: TOTAL_INCOME AS OPEN_BRACE queryFrom COMMA queryUp CLOSE_BRACE
		{ $$ = TotalIncomeQuerySemanticAction($4, $6); }
	;

totalExpensesQuery: TOTAL_EXPENSES AS OPEN_BRACE queryFrom COMMA queryUp CLOSE_BRACE
		{ $$ = TotalExpensesQuerySemanticAction($4, $6, NULL); }
	| TOTAL_EXPENSES AS OPEN_BRACE queryFrom COMMA queryUp COMMA queryCategory CLOSE_BRACE
		{ $$ = TotalExpensesQuerySemanticAction($4, $6, $8); }
	;

/* Componentes individuales de consulta */

queryFrom: FROM DATE								{ $$ = $2; }
	| FROM STRING									{ $$ = $2; }
	;

queryUp: UP DATE									{ $$ = $2; }
	| UP STRING										{ $$ = $2; }
	;

queryCategory: CATEGORY EQ STRING					{ $$ = $3; }
	| CATEGORY EQ IDENTIFIER						{ $$ = $3; }
	;

/* LISTA DE PROPIEDADES DE BLOQUE */

propertyList: %empty								{ $$ = NULL; }
	| property										{ $$ = $1; }
	| propertyList COMMA property					{ $$ = AppendPropertySemanticAction($1, $3); }
	;

property: VALUE EQ expression						{ $$ = ValuePropertySemanticAction($3); }
	| CURRENCY EQ currencyValue						{ $$ = CurrencyPropertySemanticAction($3); }
	| PERIODICITY EQ periodicityValue				{ $$ = PeriodicityPropertySemanticAction($3); }
	| CATEGORY EQ STRING							{ $$ = CategoryPropertySemanticAction($3); }
	| CATEGORY EQ IDENTIFIER						{ $$ = CategoryPropertySemanticAction($3); }
	| FROM EQ DATE									{ $$ = FromPropertySemanticAction($3); }
	| FROM EQ IDENTIFIER							{ $$ = FromPropertySemanticAction($3); }
	| UP EQ expression								{ $$ = UpPropertySemanticAction($3); }
	| BALANCE EQ expression							{ $$ = BalancePropertySemanticAction($3); }
	| INTEREST EQ expression						{ $$ = InterestPropertySemanticAction($3); }
	| MIN_PAYMENT EQ expression						{ $$ = MinPaymentPropertySemanticAction($3); }
	| AMOUNT EQ expression							{ $$ = AmountPropertySemanticAction($3); }
	| DEADLINE EQ DATE								{ $$ = DeadlinePropertySemanticAction($3); }
	| DEADLINE EQ IDENTIFIER						{ $$ = DeadlinePropertySemanticAction($3); }
	;

/* Monedas */

currencyValue: ARS									{ $$ = CURRENCY_ARS; }
	| USD											{ $$ = CURRENCY_USD; }
	;

/* Periodicidades */

periodicityValue: DAILY								{ $$ = PERIODICITY_DAILY; }
	| WEEKLY										{ $$ = PERIODICITY_WEEKLY; }
	| BIMONTHLY										{ $$ = PERIODICITY_BIMONTHLY; }
	| MONTHLY										{ $$ = PERIODICITY_MONTHLY; }
	| YEARLY										{ $$ = PERIODICITY_YEARLY; }
	;

/* EXPRESIONES ARITMÉTICAS
 * Las queries pueden participar como factores dentro de expresiones:
 *   derivatedData available = totalIncome as {...} - totalExpenses as {...};
 */

expression: expression[left] ADD expression[right]	{ $$ = ArithmeticExpressionSemanticAction($left, $right, ADDITION); }
	| expression[left] DIV expression[right]		{ $$ = ArithmeticExpressionSemanticAction($left, $right, DIVISION); }
	| expression[left] MUL expression[right]		{ $$ = ArithmeticExpressionSemanticAction($left, $right, MULTIPLICATION); }
	| expression[left] SUB expression[right]		{ $$ = ArithmeticExpressionSemanticAction($left, $right, SUBTRACTION); }
	| factor										{ $$ = FactorExpressionSemanticAction($1); }
	;

factor: OPEN_PARENTHESIS expression CLOSE_PARENTHESIS	{ $$ = ExpressionFactorSemanticAction($2); }
	| queryBlock									{ $$ = QueryFactorSemanticAction($1); }
	| constant										{ $$ = ConstantFactorSemanticAction($1); }
	;

/* CONSTANTES / LITERALES */

constant: INTEGER									{ $$ = IntegerConstantSemanticAction($1); }
	| FLOAT											{ $$ = FloatConstantSemanticAction($1); }
	| PERCENTAGE									{ $$ = PercentageConstantSemanticAction($1); }
	| STRING										{ $$ = StringConstantSemanticAction($1); }
	| DATE											{ $$ = DateConstantSemanticAction($1); }
	| IDENTIFIER									{ $$ = IdentifierConstantSemanticAction($1); }
	;

%%