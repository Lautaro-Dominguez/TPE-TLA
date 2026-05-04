%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

}

%{

/**
 * @see https://westes.github.io/flex/manual/Definitions-Section.html#Definitions-Section
 */

extern CompilationStatus ArithmeticOperatorLexemeAction(TokenLabel label);
extern CompilationStatus EnterImportExpressionLexemeAction(FlexContext context);
extern CompilationStatus EnterMultilineCommentLexemeAction(FlexContext context);
extern CompilationStatus EOFLexemeAction();
extern CompilationStatus IgnoredLexemeAction();
extern CompilationStatus IntegerLexemeAction();
extern CompilationStatus LeaveImportExpressionLexemeAction();
extern CompilationStatus LeaveMultilineCommentLexemeAction();
extern CompilationStatus ParenthesisLexemeAction(TokenLabel label);
extern CompilationStatus SubexpressionLexemeAction();
extern CompilationStatus UnknownLexemeAction();
extern CompilationStatus KeywordLexemeAction(TokenLabel label);
extern CompilationStatus RelationalOperatorLexemeAction(TokenLabel label);
extern CompilationStatus PunctuationLexemeAction(TokenLabel label);
extern CompilationStatus FloatLexemeAction(); 
extern CompilationStatus PercentageLexemeAction();
extern CompilationStatus StringLexemeAction();
extern CompilationStatus DateLexemeAction();
extern CompilationStatus IdentifierLexemeAction();

%}

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x IMPORT_EXPRESSION
%x MULTILINE_COMMENT

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
whitespace			[[:space:]]+

%%

%{

/**
 * @see https://westes.github.io/flex/manual/Rules-Section.html#Rules-Section
 */

%}

"{"									{ return EnterImportExpressionLexemeAction(IMPORT_EXPRESSION); }
<IMPORT_EXPRESSION>[^{}]+			{ return SubexpressionLexemeAction(); }
<IMPORT_EXPRESSION>"}"				{ return LeaveImportExpressionLexemeAction(); }
<IMPORT_EXPRESSION>.				{ return UnknownLexemeAction(); }

"/*"								{ return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/"				{ return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>[^*]+			{ return IgnoredLexemeAction(); }
<MULTILINE_COMMENT>.				{ return IgnoredLexemeAction(); }

"//"[^\n]*                          { return IgnoredLexemeAction(); }


"income"                            { return KeywordLexemeAction(INCOME); }
"expenses"                          { return KeywordLexemeAction(EXPENSES); }
"asset"                             { return KeywordLexemeAction(ASSET); }
"debt"                              { return KeywordLexemeAction(DEBT); }
"goal"                              { return KeywordLexemeAction(GOAL); }
"derivatedData"                     { return KeywordLexemeAction(DERIVATED_DATA); }


"as"                                { return KeywordLexemeAction(AS); }
"value"                             { return KeywordLexemeAction(VALUE); }
"currency"                          { return KeywordLexemeAction(CURRENCY); }
"periodicity"                       { return KeywordLexemeAction(PERIODICITY); }
"category"                          { return KeywordLexemeAction(CATEGORY); }
"from"                              { return KeywordLexemeAction(FROM); }
"up"                                { return KeywordLexemeAction(UP); }
"balance"                           { return KeywordLexemeAction(BALANCE); }
"interest"                          { return KeywordLexemeAction(INTEREST); }
"minPayment"                        { return KeywordLexemeAction(MIN_PAYMENT); }
"amount"                            { return KeywordLexemeAction(AMOUNT); }
"deadline"                          { return KeywordLexemeAction(DEADLINE); }


"monthly"                           { return KeywordLexemeAction(MONTHLY); }
"weekly"                            { return KeywordLexemeAction(WEEKLY); }
"daily"                             { return KeywordLexemeAction(DAILY); }
"yearly"                            { return KeywordLexemeAction(YEARLY); }
"bimonthly"                         { return KeywordLexemeAction(BIMONTHLY); }


"ARS"                               { return KeywordLexemeAction(ARS); }
"USD"                               { return KeywordLexemeAction(USD); }

"-"									{ return ArithmeticOperatorLexemeAction(SUB); }
"*"									{ return ArithmeticOperatorLexemeAction(MUL); }
"/"									{ return ArithmeticOperatorLexemeAction(DIV); }
"+"									{ return ArithmeticOperatorLexemeAction(ADD); }

"("									{ return ParenthesisLexemeAction(OPEN_PARENTHESIS); }
")"									{ return ParenthesisLexemeAction(CLOSE_PARENTHESIS); }

"<="                                { return RelationalOperatorLexemeAction(LE); }
">="                                { return RelationalOperatorLexemeAction(GE); }
"!="                                { return RelationalOperatorLexemeAction(NE); }
"<"                                 { return RelationalOperatorLexemeAction(LT); }
">"                                 { return RelationalOperatorLexemeAction(GT); }
"="                                 { return RelationalOperatorLexemeAction(EQ); }

","                                 { return PunctuationLexemeAction(COMMA); }
";"                                 { return PunctuationLexemeAction(SEMICOLON); }

\"[[:digit:]]{2}-[[:digit:]]{2}-[[:digit:]]{4}\"    { return DateLexemeAction(); }
[[:digit:]]+"."[[:digit:]]+                 { return FloatLexemeAction(); }
[[:digit:]]+"%"                     { return PercentageLexemeAction(); }
[[:digit:]]+						{ return IntegerLexemeAction(); }
\"[^\"]*\"                          { return StringLexemeAction(); }
[[:alpha:]]([[:alpha:]]|[[:digit:]])*   { return IdentifierLexemeAction(); }

{whitespace}						{ return IgnoredLexemeAction(); }
.									{ return UnknownLexemeAction(); }

<<EOF>>								{ return EOFLexemeAction(); }

%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
