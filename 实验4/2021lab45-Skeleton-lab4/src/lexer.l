%option noyywrap
%{
    /*
    * You will need to comment this line in lab5.
    */
    #define ONLY_FOR_LEX
    
    #ifdef ONLY_FOR_LEX
    #else
    #define YYSTYPE void *
    #include "parser.h"
    #endif

    #define YY_NO_UNPUT
    #define YY_NO_INPUT
    #include <string>

    #ifdef ONLY_FOR_LEX
    #include <ostream>
    #include <fstream>
    #include <iostream>
    using namespace std;
    extern FILE *yyin; 
    extern FILE *yyout;

    void DEBUG_FOR_LAB4(std::string s){
        std::string DEBUG_INFO = "[DEBUG LAB4]: \t" + s + "\n";
        fputs(DEBUG_INFO.c_str(), yyout);
    }
    #endif
%}

HEX (0x[1-9|A-F][0-9|A-F]*|0x0)
OCT (0[1-7][1-7]*|00)
DECIMIAL ([1-9][0-9]*|0)
ID [[:alpha:]_][[:alpha:][:digit:]_]*
EOL (\r\n|\n|\r)
WHITE [\t ]
linecomment (\/\/.*|\/\/.*)
commentbegin (\/\*[\n]|\/\*)
commentelement .*
commentend ([\t]*\*\/|[\n][\t]*\*\/)
%x BLOCKCOMMENT

%%


{linecomment} {
    #ifdef ONLY_FOR_LEX
        string a = yytext;
        string temp = "linecomment\t" + a + "\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else 
        return linecomment;
    #endif
}

{commentbegin} {
    #ifdef ONLY_FOR_LEX
        BEGIN BLOCKCOMMENT;
        string a = yytext;

        while(a.find('\n') != a.npos)
        {
            a.replace(a.find('\n'),1,"");
        }
        string temp = "commentbegin\t/*\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else 
        return commentbegin;
    #endif
}

<BLOCKCOMMENT>{commentend} {
    #ifdef ONLY_FOR_LEX
        string a = yytext;
        if(a.find("\n") != a.npos)
        {
            yylineno++;
            a.replace(a.find("\n"),1,"");
        }
        string temp = "commentend\t" + a + "\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
        BEGIN INITIAL;
    #else 
        return commentend;
    #endif
}


<BLOCKCOMMENT>{commentelement} {
    #ifdef ONLY_FOR_LEX
        string a = yytext;
        yylineno++;
        if(a.find('\n') != a.npos)
        {
            a.replace(a.find('\n'),1,"");
        }
        string temp = "commentelement\t" + a + "\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else 
        return commentelement;
    #endif
}



"int" {
    /*
    * Questions: 
    *   Q1: Why we need to return INT in further labs?
    *   Q2: What is "INT" actually?
    */
    #ifdef ONLY_FOR_LEX
        string temp = "INT\tint\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return INT;
    #endif
}
"char" {
    #ifdef ONLY_FOR_LEX        
        string temp = "CHAR\tchar\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return CHAR;
    #endif
}
"while" {
    #ifdef ONLY_FOR_LEX
        string temp = "WHILE\twhile\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return WHILE;
    #endif
}
"break" {
    #ifdef ONLY_FOR_LEX
        string temp = "BREAK\tbreak\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return BREAK;
    #endif
}
"continue" {
    #ifdef ONLY_FOR_LEX
        string temp = "CONTINUE\tcontinue\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return CONTINUE;
    #endif
}
"const" {
    #ifdef ONLY_FOR_LEX
        string temp = "CONST\tconst\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return CONST;
    #endif
}
"new" {
    #ifdef ONLY_FOR_LEX
        string temp = "NEW\tnew\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return NEW;
    #endif
}
"delete" {
    #ifdef ONLY_FOR_LEX
        string temp = "DELETE\tdelete\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return DELETE;
    #endif
}
"using" {
    #ifdef ONLY_FOR_LEX
        string temp = "USING\tusing\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return USING;
    #endif
}
"namespace" {
    #ifdef ONLY_FOR_LEX
        string temp = "NAMESPACE\namespace\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return namespace;
    #endif
}
"void" {
    #ifdef ONLY_FOR_LEX
        string temp = "VOID\tvoid\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return VOID;
    #endif 
}
"if" {
    #ifdef ONLY_FOR_LEX
        string temp = "IF\tif\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return IF;
    #endif
};
"else" {
    #ifdef ONLY_FOR_LEX
        string temp = "ELSE\telse\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return ELSE;
    #endif
};
"return" {
    #ifdef ONLY_FOR_LEX
        string temp = "RETURN\treturn\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return RETURN;
    #endif
}

"==" {
    #ifdef ONLY_FOR_LEX
        string temp = "EQUAL\t==\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return EQUAL;
    #endif
}
"=" {
    #ifdef ONLY_FOR_LEX
        string temp = "ASSIGN\t=\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return ASSIGN;
    #endif
}
"!=" {
    #ifdef ONLY_FOR_LEX
        string temp = "UNEQUAL\t!=\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return UNEQUAL;
    #endif
}
"!" {
    #ifdef ONLY_FOR_LEX
        string temp = "AGAINST\t!\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return AGAINST;
    #endif
}
"*" {
    #ifdef ONLY_FOR_LEX
        string temp = "STAR\t*\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return STAR;
    #endif
}
"<" {
    #ifdef ONLY_FOR_LEX
        string temp = "LESS\t<\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return LESS;
    #endif
}
">" {
    #ifdef ONLY_FOR_LEX
        string temp = "MORE\t>\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return MORE;
    #endif
}
"+" {
    #ifdef ONLY_FOR_LEX
        string temp = "ADD\t+\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return ADD;
    #endif
}
"-" {
    #ifdef ONLY_FOR_LEX
        string temp = "SUB\t-\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return SUB;
    #endif
}
"*" {
    #ifdef ONLY_FOR_LEX
        string temp = "MUL\t*\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return MUL;
    #endif
}
"/" {
    #ifdef ONLY_FOR_LEX
        string temp = "DIV\t/\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return DIV;
    #endif
}
"%" {
    #ifdef ONLY_FOR_LEX
        string temp = "MOD\t%\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return MOD;
    #endif
}
";" {
    #ifdef ONLY_FOR_LEX
        string temp = "SEMICOLON\t;\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return SEMICOLON;
    #endif
}
"(" {
    #ifdef ONLY_FOR_LEX
        string temp = "LPAREN\t(\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else 
        return LPAREN; 
    #endif
}
")" {
    #ifdef ONLY_FOR_LEX
        string temp = "RPAREN\t)\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return RPAREN;
    #endif
}
"{" {
    #ifdef ONLY_FOR_LEX
        string temp = "LBRACE\t{\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return LBRACE;
    #endif
}
"}" {
    #ifdef ONLY_FOR_LEX
        string temp = "RBRACE\t}\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return RBRACE;
    #endif
}
"[" {
    #ifdef ONLY_FOR_LEX
        string temp = "LSQUARE\t[\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return LSQUARE;
    #endif
}
"]" {
    #ifdef ONLY_FOR_LEX
        string temp = "RSQUARE\t]\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return RSQUARE;
    #endif
}
{ID} {
    #ifdef ONLY_FOR_LEX
        string a = yytext;
        string temp = "ID\t" + a + "\tlinenum:" + to_string(yylineno);;
        DEBUG_FOR_LAB4(temp);
    #else 
        return ID;
    #endif
}

{DECIMIAL} {
    #ifdef ONLY_FOR_LEX
        string a = yytext;
        string temp = "DECIMIAL\t" + a + "\tlinenum:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else 
        return DECIMIAL;
    #endif
}


{EOL} yylineno++;
{WHITE}
%%

#ifdef ONLY_FOR_LEX
int main(int argc, char **argv){
    if(argc != 5){
        fprintf(stderr, "Argument Not Enough");
        exit(EXIT_FAILURE);
    }

    if(!(yyin = fopen(argv[1], "r"))){
        fprintf(stderr, "No such file or directory: %s", argv[1]);
        exit(EXIT_FAILURE);
    }

    if(!(yyout = fopen(argv[3], "w"))){
        fprintf(stderr, "No such file or directory: %s", argv[3]);
        exit(EXIT_FAILURE);
    }
    yylex();
    return 0;
}
#endif
