#include "Generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <errno.h>


static Logger * _logger = NULL;

static void _shutdownGeneratorModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: Generator...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeGeneratorModule() {
	_logger = createLogger("Generator");
	return _shutdownGeneratorModule;
}


#define OUTPUT_DIR  "output/src/main/java/ar/edu/itba/atlyc"
#define OUTPUT_PATH "output/src/main/java/ar/edu/itba/atlyc/Main.java"

static FILE * _outputFile = NULL;

static void _out(const char * fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(_outputFile, fmt, args);
	va_end(args);
}

static void _generateLocalDate(const char * date) {
	int dd, mm, yyyy;
	if (date != NULL && sscanf(date, "%d-%d-%d", &dd, &mm, &yyyy) == 3) {
		_out("LocalDate.of(%d, %d, %d)", yyyy, mm, dd);
	} else {
		_out("LocalDate.now()");
	}
}

static void _generateCurrency(CurrencyType c) {
	_out("Currency.getInstance(\"%s\")", c == CURRENCY_ARS ? "ARS" : "USD");
}

static void _generatePeriodicity(PeriodicityType p) {
	switch (p) {
		case PERIODICITY_DAILY:     _out("Period.ofDays(1)");    break;
		case PERIODICITY_WEEKLY:    _out("Period.ofWeeks(1)");   break;
		case PERIODICITY_BIMONTHLY: _out("Period.ofMonths(2)");  break;
		case PERIODICITY_MONTHLY:   _out("Period.ofMonths(1)");  break;
		case PERIODICITY_YEARLY:    _out("Period.ofYears(1)");   break;
		default:                    _out("Period.ofMonths(1)");  break;
	}
}

static const char * _relOpStr(RelationalOpType op) {
	switch (op) {
		case RELOP_LT: return "<";
		case RELOP_GT: return ">";
		case RELOP_EQ: return "==";
		case RELOP_NE: return "!=";
		case RELOP_LE: return "<=";
		case RELOP_GE: return ">=";
		default:       return ">";
	}
}


static void _generateExpression(Expression * expr);

static void _generateConstant(Constant * c) {
	switch (c->type) {
		case INTEGER_CONSTANT:    _out("%d", c->intValue);          break;
		case FLOAT_CONSTANT:      _out("%.2f", c->floatValue);      break;
		case PERCENTAGE_CONSTANT: _out("%.6f", c->floatValue);      break;
		case STRING_CONSTANT:     _out("\"%s\"", c->stringValue);   break;
		case DATE_CONSTANT:       _generateLocalDate(c->stringValue); break;
		case IDENTIFIER_CONSTANT: _out("%s", c->stringValue);       break;
		default: break;
	}
}

static void _generateQuery(QueryBlock * query) {
	switch (query->type) {
		case QUERY_TOTAL_INCOME:
			if (query->from != NULL && query->up != NULL) {
				_out("UtilFunctions.getTotalIncome(incomes, ");
				_generateLocalDate(query->from);
				_out(", ");
				_generateLocalDate(query->up);
				_out(")");
			} else {
				_out("UtilFunctions.getTotalIncome(incomes)");
			}
			break;
		case QUERY_TOTAL_EXPENSES:
			if (query->from != NULL && query->up != NULL) {
				if (query->category != NULL) {
					_out("UtilFunctions.getTotalExpense(expenses, ");
					_generateLocalDate(query->from);
					_out(", ");
					_generateLocalDate(query->up);
					_out(", \"%s\")", query->category);
				} else {
					_out("UtilFunctions.getTotalExpense(expenses, ");
					_generateLocalDate(query->from);
					_out(", ");
					_generateLocalDate(query->up);
					_out(")");
				}
			} else {
				if (query->category != NULL) {
					_out("UtilFunctions.getTotalExpense(expenses, \"%s\")", query->category);
				} else {
					_out("UtilFunctions.getTotalExpense(expenses)");
				}
			}
			break;
		case QUERY_MAX_CATEGORY:
			_out("UtilFunctions.getMaxCategory(expenses)");
			break;
		case QUERY_MIN_CATEGORY:
			_out("UtilFunctions.getMinCategory(expenses)");
			break;
		default: break;
	}
}

static void _generateFactor(Factor * f) {
	switch (f->type) {
		case CONSTANT:
			_generateConstant(f->constant);
			break;
		case EXPRESSION:
			_out("(");
			_generateExpression(f->expression);
			_out(")");
			break;
		case QUERY:
			_generateQuery(f->query);
			break;
		default: break;
	}
}

static void _generateExpression(Expression * expr) {
	switch (expr->type) {
		case ADDITION:
			_generateExpression(expr->leftExpression);
			_out(" + ");
			_generateExpression(expr->rightExpression);
			break;
		case SUBTRACTION:
			_generateExpression(expr->leftExpression);
			_out(" - ");
			_generateExpression(expr->rightExpression);
			break;
		case MULTIPLICATION:
			_generateExpression(expr->leftExpression);
			_out(" * ");
			_generateExpression(expr->rightExpression);
			break;
		case DIVISION:
			_generateExpression(expr->leftExpression);
			_out(" / ");
			_generateExpression(expr->rightExpression);
			break;
		case FACTOR:
			_generateFactor(expr->factor);
			break;
		default: break;
	}
}


static Property * _findProperty(Property * list, PropertyType type) {
	while (list != NULL) {
		if (list->type == type) return list;
		list = list->next;
	}
	return NULL;
}

static const char * _extractDateStr(Expression * expr) {
	if (expr == NULL || expr->type != FACTOR) return NULL;
	Factor * f = expr->factor;
	if (f->type != CONSTANT) return NULL;
	if (f->constant->type == DATE_CONSTANT || f->constant->type == STRING_CONSTANT)
		return f->constant->stringValue;
	return NULL;
}


#define MAX_BALANCE_NAMES 64
static const char * _balanceNames[MAX_BALANCE_NAMES];
static int _balanceCount = 0;

static int _isBalance(const char * name) {
	if (name == NULL) return 0;
	for (int i = 0; i < _balanceCount; i++) {
		if (_balanceNames[i] && strcmp(_balanceNames[i], name) == 0) return 1;
	}
	return 0;
}

static void _collectBalanceNames(Program * program) {
	_balanceCount = 0;
	Statement * stmt = program->statements;
	while (stmt != NULL) {
		if (stmt->type == STATEMENT_DECLARATION) {
			Declaration * d = stmt->declaration;
			if (d->type == DECLARATION_BALANCE && d->name != NULL && _balanceCount < MAX_BALANCE_NAMES) {
				_balanceNames[_balanceCount++] = d->name;
			}
		}
		stmt = stmt->next;
	}
}


static void _generateDeclaration(Declaration * decl) {
	switch (decl->type) {
		case DECLARATION_INCOME: {
			Property * val = _findProperty(decl->properties, PROPERTY_VALUE);
			Property * cur = _findProperty(decl->properties, PROPERTY_CURRENCY);
			Property * per = _findProperty(decl->properties, PROPERTY_PERIODICITY);
			_out("        Income %s = new Income(", decl->name);
			if (val) _generateExpression(val->valueExpr); else _out("0.0");
			_out(", ");
			if (cur) _generateCurrency(cur->currencyValue); else _out("Currency.getInstance(\"ARS\")");
			_out(", ");
			if (per) _generatePeriodicity(per->periodicityValue); else _out("Period.ofMonths(1)");
			_out(", \"%s\");\n", decl->name);
			_out("        incomes.add(%s);\n", decl->name);
			break;
		}
		case DECLARATION_EXPENSES: {
			Property * val = _findProperty(decl->properties, PROPERTY_VALUE);
			Property * cur = _findProperty(decl->properties, PROPERTY_CURRENCY);
			Property * per = _findProperty(decl->properties, PROPERTY_PERIODICITY);
			Property * cat = _findProperty(decl->properties, PROPERTY_CATEGORY);
			_out("        Expense %s = new Expense(", decl->name);
			if (val) _generateExpression(val->valueExpr); else _out("0.0");
			_out(", ");
			if (cur) _generateCurrency(cur->currencyValue); else _out("Currency.getInstance(\"ARS\")");
			_out(", ");
			if (per) _generatePeriodicity(per->periodicityValue); else _out("Period.ofMonths(1)");
			_out(", ");
			if (cat) _out("\"%s\"", cat->stringValue); else _out("\"\"");
			_out(", \"%s\");\n", decl->name);
			_out("        expenses.add(%s);\n", decl->name);
			break;
		}
		case DECLARATION_ASSET: {
			Property * val = _findProperty(decl->properties, PROPERTY_VALUE);
			Property * cur = _findProperty(decl->properties, PROPERTY_CURRENCY);
			_out("        Asset %s = new Asset(", decl->name);
			if (val) _generateExpression(val->valueExpr); else _out("0.0");
			_out(", ");
			if (cur) _generateCurrency(cur->currencyValue); else _out("Currency.getInstance(\"ARS\")");
			_out(", \"%s\");\n", decl->name);
			_out("        assets.add(%s);\n", decl->name);
			break;
		}
		case DECLARATION_GOAL: {
			Property * val  = _findProperty(decl->properties, PROPERTY_VALUE);
			Property * cur  = _findProperty(decl->properties, PROPERTY_CURRENCY);
			Property * dead = _findProperty(decl->properties, PROPERTY_DEADLINE);
			_out("        Goal %s = new Goal(", decl->name);
			if (val) _generateExpression(val->valueExpr); else _out("0.0");
			_out(", ");
			if (cur) _generateCurrency(cur->currencyValue); else _out("Currency.getInstance(\"ARS\")");
			_out(", ");
			if (dead) _generateLocalDate(dead->stringValue); else _out("LocalDate.now()");
			_out(", \"%s\");\n", decl->name);
			_out("        goals.add(%s);\n", decl->name);
			break;
		}
		case DECLARATION_DEBT: {
			Property * val  = _findProperty(decl->properties, PROPERTY_VALUE);
			Property * cur  = _findProperty(decl->properties, PROPERTY_CURRENCY);
			Property * cat  = _findProperty(decl->properties, PROPERTY_CATEGORY);
			Property * dead = _findProperty(decl->properties, PROPERTY_DEADLINE);
			Property * intr = _findProperty(decl->properties, PROPERTY_INTEREST);
			Property * minp = _findProperty(decl->properties, PROPERTY_MIN_PAYMENT);
			_out("        Debt %s = new Debt(", decl->name);
			if (val) _generateExpression(val->valueExpr); else _out("0.0");
			_out(", ");
			if (cur) _generateCurrency(cur->currencyValue); else _out("Currency.getInstance(\"ARS\")");
			_out(", ");
			if (cat) _out("\"%s\"", cat->stringValue); else _out("\"\"");
			_out(", ");
			if (dead) _generateLocalDate(dead->stringValue); else _out("LocalDate.now()");
			_out(", ");
			if (intr) _generateExpression(intr->interestExpr); else _out("0.0");
			_out(", ");
			if (minp) _generateExpression(minp->minPaymentExpr); else _out("0.0");
			_out(", \"%s\");\n", decl->name);
			_out("        debts.add(%s);\n", decl->name);
			break;
		}
		case DECLARATION_DERIVATED_DATA: {
			Property * val = _findProperty(decl->properties, PROPERTY_VALUE);
			if (val != NULL) {
				_out("        double %s = ", decl->name);
				_generateExpression(val->valueExpr);
				_out(";\n");
				_out("        System.out.println(%s);\n", decl->name);
			}
			break;
		}
		case DECLARATION_DERIVATED_EXPR: {
			if (decl->name != NULL) {
				_out("        double %s = ", decl->name);
				_generateExpression(decl->derivedExpr);
				_out(";\n");
				_out("        System.out.println(%s);\n", decl->name);
			} else {
				/* standalone query statement */
				_out("        System.out.println(");
				_generateExpression(decl->derivedExpr);
				_out(");\n");
			}
			break;
		}
		case DECLARATION_BALANCE: {
			Property * fromProp = _findProperty(decl->properties, PROPERTY_FROM);
			Property * upProp   = _findProperty(decl->properties, PROPERTY_UP);
			const char * fromStr = (fromProp != NULL) ? fromProp->stringValue : NULL;
			const char * upStr   = (upProp != NULL) ? _extractDateStr(upProp->upExpr) : NULL;
			_out("        List<List<? extends EconomicVariable>> _vars_%s = new ArrayList<>();\n", decl->name);
			_out("        _vars_%s.add(incomes);\n", decl->name);
			_out("        _vars_%s.add(expenses);\n", decl->name);
			_out("        _vars_%s.add(goals);\n", decl->name);
			_out("        _vars_%s.add(debts);\n", decl->name);
			_out("        _vars_%s.add(assets);\n", decl->name);
			_out("        List<EconomicVariable> %s = UtilFunctions.balance(", decl->name);
			_generateLocalDate(fromStr);
			_out(", ");
			_generateLocalDate(upStr);
			_out(", _vars_%s);\n", decl->name);
			break;
		}
		default: break;
	}
}


static void _generateCommand(Command * cmd) {
	switch (cmd->type) {
		case COMMAND_EXCHANGE:
			_out("        %s.exchange(", cmd->targetName);
			_generateCurrency(cmd->newCurrency);
			_out(");\n");
			break;
		case COMMAND_CHANGE_PERIODICITY:
			_out("        %s.changePeriodicity(", cmd->targetName);
			_generatePeriodicity(cmd->newPeriodicity);
			_out(");\n");
			break;
		case COMMAND_EXPORT:
			_out("        UtilFunctions.export(%s);\n", cmd->targetName);
			break;
		case COMMAND_CIRCLE_GRAPHIC:
		case COMMAND_CIRCLE_GRAPHIC_BY_CATEGORY:
			_out("        UtilFunctions.circleGraphic(expenses);\n");
			break;
		case COMMAND_PROBABILITY:
			_out("        System.out.println(");
			_generateQuery(cmd->query);
			_out(" %s %.2f);\n", _relOpStr(cmd->relationalOp), cmd->threshold);
			break;
		case COMMAND_PLAN:
			_out("        System.out.println(%s.plan(%d));\n", cmd->targetName, cmd->installments);
			break;
		default: break;
	}
}


static void _generatePrologue(void) {
	_out("package ar.edu.itba.atlyc;\n\n");
	_out("import java.time.LocalDate;\n");
	_out("import java.time.Period;\n");
	_out("import java.util.ArrayList;\n");
	_out("import java.util.Currency;\n");
	_out("import java.util.List;\n\n");
	_out("public class Main {\n");
	_out("    public static void main(String[] args) {\n");
	_out("        List<Income> incomes = new ArrayList<>();\n");
	_out("        List<Expense> expenses = new ArrayList<>();\n");
	_out("        List<Goal> goals = new ArrayList<>();\n");
	_out("        List<Debt> debts = new ArrayList<>();\n");
	_out("        List<Asset> assets = new ArrayList<>();\n\n");
}

static void _generateEpilogue(void) {
	_out("    }\n");
	_out("}\n");
}

static void _generateStatements(Program * program) {
	Statement * stmt = program->statements;
	while (stmt != NULL) {
		if (stmt->type == STATEMENT_DECLARATION) {
			_generateDeclaration(stmt->declaration);
		} else if (stmt->type == STATEMENT_COMMAND) {
			_generateCommand(stmt->command);
		}
		stmt = stmt->next;
	}
}

static void _mkdirp(const char * path) {
	char tmp[256];
	strncpy(tmp, path, sizeof(tmp) - 1);
	tmp[sizeof(tmp) - 1] = '\0';
	for (char * p = tmp + 1; *p; p++) {
		if (*p == '/') {
			*p = '\0';
			mkdir(tmp, 0755);
			*p = '/';
		}
	}
	mkdir(tmp, 0755);
}


void executeGenerator(CompilerState * compilerState) {
	logDebugging(_logger, "Generating Java output...");
	Program * program = (Program *) compilerState->abstractSyntaxtTree;
	if (program == NULL) {
		logError(_logger, "No AST available for code generation.");
		return;
	}
	_mkdirp(OUTPUT_DIR);
	_outputFile = fopen(OUTPUT_PATH, "w");
	if (_outputFile == NULL) {
		logError(_logger, "Could not open output file: %s", OUTPUT_PATH);
		return;
	}
	_collectBalanceNames(program);
	_generatePrologue();
	if (program->statements != NULL) {
		_generateStatements(program);
	}
	_generateEpilogue();
	fclose(_outputFile);
	_outputFile = NULL;
	logDebugging(_logger, "Code generation done. Output written to: %s", OUTPUT_PATH);
}
