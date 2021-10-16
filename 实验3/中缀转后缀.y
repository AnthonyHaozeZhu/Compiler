%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef YYSTYPE
#define YYSTYPE char*
#endif
char idStr[50];
char numStr[50];
int yylex();
extern int yyparse();
FILE* yyin;
void yyerror(const char* s);
%}

%token id 
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
lines   :   lines expr ';' {printf("%s\n", $2);}
        |   lines ';'
	|
        ;

expr    :   expr add expr {$$ = (char*)malloc(50 * sizeof(char));
                            strcpy($$, $1);
                            strcat($$, $3);
                            strcat($$, "+");
                          }
        |   expr sub expr {$$ = (char*)malloc(50 * sizeof(char));
                            strcpy($$, $1);
                            strcat($$, $3);
                            strcat($$, "-");
                          }
        |   expr mul expr{$$ = (char*)malloc(50 * sizeof(char));
                            strcpy($$, $1);
                            strcat($$, $3);
                            strcat($$, "*");
                          }
        |   l_bracket expr r_barcket  {$$ = $2;}
        |   expr DIV expr{$$ = (char*)malloc(50 * sizeof(char));
                            strcpy($$, $1);
                            strcat($$, $3);
                            strcat($$, "/");
                          }
        |   sub expr %prec UMINUS{$$ = (char*)malloc(50 * sizeof(char));
                                    strcpy($$, $2);
                                    strcat($$, "-");
                                 }
        |   NUMBER {$$ = (char*)malloc(50 * sizeof(char));
                    strcpy($$, $1);
                    strcat($$, " ");
                    }
        |   id {$$ = (char*)malloc(50 * sizeof(char));
                strcpy($$, $1);
                strcat($$, " ");
                }
        ;
%%


int yylex()
{
    int t;
    while(1){
        t = getchar();
        if(t == ' '|| t == '\t' || t == '\n'){
            
        }
        else if((t >= '0' && t <= '9')){
            int ti = 0;
            while((t >= '0' && t <= '9')){
                numStr[ti]= t;
                t = getchar();
                ti++;
            }
            numStr[ti] = '\0';
            yylval = numStr;
            ungetc(t, stdin);
            return NUMBER;
        }
        /*
        else if((t >= 'a' && t <= 'z') || (t >= 'A' && t <= 'Z') || t == '_'){
            int ti = 0;
            whlie((t >= 'a' && t <= 'z') || (t >= 'A' && t <= 'Z') || (t == '_') || (t >= '0' && t <= '9'))
            {
                idStr[ti] = t;
                ti++;
                t = getchar();
            }
            idStr[ti]= '\0';
            yylval = idStr;
            ungetc(t, stdin);
            return id;
        }
        */
        else if((t >= 'a' && t <= 'z') || (t >= 'A' && t <= 'Z') || (t == '_')){
            int ti = 0;
            while((t >= 'a' && t <= 'z') || (t >= 'A' && t <= 'Z') || (t == '_') || (t >= '0' || t <= '9')){
                idStr[ti] = t;
                ti++;
                t = getchar();
            }
            idStr[ti] = '\0';
            yylval = idStr;
            ungetc(t, stdin);
            return id;
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
