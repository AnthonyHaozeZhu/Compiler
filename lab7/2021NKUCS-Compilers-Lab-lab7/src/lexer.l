%option noyywrap
%{
    #define YY_NO_UNPUT
    #define YY_NO_INPUT

    #include "parser.h"
    #include <ostream>
    #include <fstream>
    using namespace std;

    extern FILE *yyin; 
    extern FILE *yyout;
    extern bool dump_tokens;

    void DEBUG_FOR_LAB4(std::string s){
        std::string DEBUG_INFO = "[DEBUG LAB4]: \t" + s + "\n";
        fputs(DEBUG_INFO.c_str(), yyout);
    }
%}

DECIMIAL [1-9][0-9]*
OCTAL 0[0-7]*
HEXADECIMAL 0(x|X)[0-9a-zA-Z]+
ID [[:alpha:]_][[:alpha:][:digit:]_]*
EOL (\r\n|\n|\r)
WHITE [\t ]
LINECOMMENT \/\/[^\n]*
BLOCKCOMMENT_BEGIN "/*"
BLOCKCOMMENT_ELE .
BLOCKCOMMENT_LINE (\r\n|\n|\r)
BLOCKCOMMENT_END "*/"
%x BLOCKCOMMENT

%%

"int" return INT;
"void" return VOID;
"const" return CONST;

"if" return IF;
"while" return WHILE;
"else" return ELSE;
"return" return RETURN;
"=" return ASSIGN;
"+" return ADD;
"-" return SUB;
"*" return MUL;
"/" return DIV;
"%" return MOD;
"!" return NOT;
"||" return OR;
"&&" return AND;
"!=" return NOTEQUAL;
"==" return EQUAL;
"<" return LESS;
">" return GREATER;
"<=" return LESSEQUAL;
">=" return GREATEREQUAL;
"," return COMMA;
";" return SEMICOLON;
"(" return LPAREN;
")" return RPAREN;
"{" return LBRACE;
"}" return RBRACE;
"[" return LBRACKET;
"]" return RBRACKET;

{DECIMIAL} {
    if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    yylval.itype = atoi(yytext);
    return INTEGER;
}

{OCTAL} {
    int val;
    sscanf(yytext, "%o", &val);
    yylval.itype = val;
    return INTEGER;
}
{HEXADECIMAL} {
    int val;
    sscanf(yytext, "%x", &val);
    yylval.itype = val;
    return INTEGER;
}

{ID} {
    if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    return ID;
}
{EOL} yylineno++;
{WHITE}
{LINECOMMENT}
{BLOCKCOMMENT_BEGIN} {BEGIN BLOCKCOMMENT;}
<BLOCKCOMMENT>{BLOCKCOMMENT_ELE} {}
<BLOCKCOMMENT>{BLOCKCOMMENT_LINE} {yylineno++;}
<BLOCKCOMMENT>{BLOCKCOMMENT_END} {BEGIN INITIAL;}
%%
