%{
#include <stdio.h>
#include <stdlib.h>
#ifndef YYSTYPE
#define YYSTYPE double 
#endif
int yylex();
extern int yyparse();
FILE *yyin;
void yyerror(const char* s);
%}

%token add 
%token sub 
%token mul 
%token DIV 
%token l_bracket 
%token r_barcket 
%token NUMBER
%left add sub
%left mul DIV
%right UMINUS

%%

lines		:	lines expr ';' {printf("%f\n", $2);}
		|	lines ';'
		|
		;

expr		:	expr add expr {$$ = $1 + $3;}
		|	expr sub expr {$$ = $1 - $3;}
		|	expr mul expr {$$ = $1 * $3;}
		|	expr DIV expr {$$ = $1 / $3;}
		|   l_bracket expr r_barcket  {$$ = $2;}
		|	sub expr %prec UMINUS {$$ = -$2;}
		|	NUMBER {$$ = $1;}
		;

/*
NUMBER	:	'0'		{$$ = 0.0;}
		|	'1'		{$$ = 1.0;}
		|	'2'		{$$ = 2.0;}
		|	'3'		{$$ = 3.0;}
		|	'4'		{$$ = 4.0;}
		|	'5'		{$$ = 5.0;}
		|	'6'		{$$ = 6.0;}
		|	'7'		{$$ = 7.0;}
		|	'8'		{$$ = 8.0;}
		|	'9'		{$$ = 9.0;}
		;
*/
//#include <stdlib.h>
%%

int yylex()
{
	int t;
	while(1){
		t = getchar();
		if(t == ' ' || t == '\t' || t == '\n'){

		}
		else if(isdigit(t))
		{
			yylval = 0;
			while(isdigit(t)){
				yylval = yylval * 10 + t - '0';
				t = getchar();
			}
			ungetc(t, stdin);
			return NUMBER;
		}
		else if(t == '+'){
			return add;
		}
		else if(t == '-'){
			return sub;
		}
		else if(t == '*'){
			return mul;
		}
		else if(t == '/'){
			return DIV;
		}
		else if(t == '('){
			return l_bracket;
		}
		else if(t == ')'){
			return r_barcket;
		}
		else{
			return t;
		}
	}
}

int main(void)
{
	yyin = stdin;
	do{
		yyparse();
	}while(!feof(yyin));
	return 0;
}

void yyerror(const char* s){
	fprintf(stderr, "Parse error:%s\n", s);
	exit(1);
}
