%{

#include <stdlib.h>
#include <string.h>
#include <string>

/* This is where you put #include directives as needed for later labs. */
#include "scanner.hh"

// Note that the order is important
// include ast.hh
// include parser.hh

int column = 0;
std::stringstream string_buf;
extern symbol_table * sym_tab;

extern YYLTYPE yylloc; // Used for position information, see below.

%}

%option yylineno
%option 8bit
%option noyywrap
%option caseless
%option nodefault
%option outfile = "scanner.cc"
/* Your code here.
 * If you want to include any flex declarations, this is where to do it. */
%x c_comment
%x d_comment
%x string

DIGIT [0-9]
INTEGER {DIGIT}+
	       //REAL {DIGIT}*"."{DIGIT}*(e[+-]?{DIGIT}+)?
EXPOSANT e[+-]?{DIGIT}+
REAL {DIGIT}+"."{DIGIT}*{EXPOSANT}?|{DIGIT}+{EXPOSANT}|"."{DIGIT}+{EXPOSANT}?
ID [a-z_][a-z0-9_]*
STRINGCHAR [^\'\n]|\'\'
/* Position information is entered into the yylloc variable, so we will be
   able to generate good error messages later in later phases. */
/* Your code should be entered below the %%. Expressions to handle the 
   following: Diesel comments, Diesel string constants, Diesel 
   identifiers, integers, reals, and whitespace. */
%%

\.				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_DOT;
				}
;				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_SEMICOLON;
				}
=				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_EQ;			    
				}
\:				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_COLON; 
				}
\(				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_LEFTPAR;
				}
\)				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_RIGHTPAR;
				}
\[				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_LEFTBRACKET;   
				}
\]				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_RIGHTBRACKET;    
				}
,				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_COMMA;
				    
				}
\<				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_LESSTHAN;
				}
\>				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_GREATERTHAN;
				}
\+				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_ADD;				    
				}
\-				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_SUB;	
				}
\*				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_MUL;	
				}
\/				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_RDIV;	
				}
":="				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_ASSIGN;	
				}
"<>"				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_NOTEQ;	
				}
of				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_OF;	
				}
if				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_IF;	
				}
do				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_DO;	
				}
or				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_OR;	
				}
var				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_VAR;	
				}
end				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_END;	
				}
and				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_AND;	
				}
div				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_IDIV;	
				}
mod				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_MOD;	
				}
not				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_NOT;	
				}
then				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_THEN;	
				}
else				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_ELSE;	
				}
const				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_CONST;	
				}
array				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_ARRAY;	
				}
begin				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_BEGIN;	
				}
while				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_WHILE;	
				}
elsif				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_ELSIF;	
				}
return				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_RETURN;	
				}
program				{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_PROGRAM;	
				}
function			{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_FUNCTION;	
				}
procedure			{yylloc.first_line=yylineno;
 				 yylloc.first_column=column;
                         column += yyleng;
                         return T_PROCEDURE;	
				}

{REAL}		{
                   yylloc.first_line=yylineno;
                   yylloc.first_column=column;
		   column += yyleng;
		   yylval.rval = atof(yytext);
		   return T_REALNUM;
                }

{INTEGER}	{
                   yylloc.first_line=yylineno;
                   yylloc.first_column=column;
		   column += yyleng;
		   yylval.ival = atoi(yytext);
		   return T_INTNUM;
                }

{ID}		{
                                    yylloc.first_line=yylineno;
				    yylloc.first_column=column;
				    column += yyleng;
				    char* name = strdup(yytext);
				    for (int i = 0; i < yyleng; ++i)
				      name[i] = toupper(yytext[i]);
				    yylval.pool_p = sym_tab->pool_install(name);
				    free(name);
				    return T_IDENT;
				}

\/\/.*$				column = 0; /* Skip single-line comment */
"/\*"				{
                                    column += yyleng;
                                    BEGIN(c_comment);
                                }
\{                              {
                                    column += yyleng;
                                    BEGIN(d_comment);
                                }


				/*\'([^\']|\'\')+\' {*/
  \'{STRINGCHAR}*\' {
                                    yylloc.first_line=yylineno;
				    yylloc.first_column=column;
				    column += yyleng;
				    int size = strlen(yytext);
				    char* s = (char*)malloc(size - 2);
				    int j = 0;
				    for (int i = 1; i < size - 1; ++i, ++j)
				      {
					s[j] = yytext[i];
					if (yytext[i] == '\'')
					  ++i;
				      }
				    s[j] = 0;
				    yylval.pool_p = sym_tab->pool_install(s);
				    free(s);
				    return T_STRINGCONST;
}
\'{STRINGCHAR}*\n                   yyerror("Newline in string");


<c_comment>
{
    "\*/"			{
                                    column += 2;
                                    BEGIN(INITIAL);
                                }
    "/\*"			{
				    column += 2;
				    yyerror("Suspicious comment");
				}
    [^\n]			column++; /* Skip stuff in comments */
    \n				column = 0; 
    <<EOF>>			{
				    yyerror("Unterminated comment");
				    yyterminate();
				}
}

<d_comment>
{
    "}"                         {
                                    column += 2;
                                    BEGIN(INITIAL);
                                }
    [^\n]                       column++;
    \n                          column = 0;
    <<EOF>>                     {
                                    yyerror("Unterminated comment");
                                    yyterminate();
                                }
}

" "|\t                          column++;
\n                              column = 0;

<<EOF>>				yyterminate();
.				yyerror("Illegal character");
