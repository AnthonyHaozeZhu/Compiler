#include <iostream>
#include <string.h>
#include <unistd.h>
#include "Ast.h"
#include "Type.h"
#include "SymbolTable.h"
using namespace std;

Ast ast;
extern FILE *yyin;
extern FILE *yyout;

int yyparse();

char outfile[256] = "a.out";
bool dump_tokens;
bool dump_ast;

int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "ato:")) != -1)
    {
        switch (opt)
        {
        case 'o':
            strcpy(outfile, optarg);
            break;
        case 'a':
            dump_ast = true;
            break;
        case 't':
            dump_tokens = true;
            break;
        default:
            fprintf(stderr, "Usage: %s [-o outfile] infile\n", argv[0]);
            exit(EXIT_FAILURE);
            break;
        }
    }
    if (optind >= argc)
    {
        fprintf(stderr, "no input file\n");
        exit(EXIT_FAILURE);
    }
    if (!(yyin = fopen(argv[optind], "r")))
    {
        fprintf(stderr, "%s: No such file or directory\nno input file\n", argv[optind]);
        exit(EXIT_FAILURE);
    }
    if (!(yyout = fopen(outfile, "w")))
    {
        fprintf(stderr, "%s: fail to open output file\n", outfile);
        exit(EXIT_FAILURE);
    }
    SymbolEntry *se1 = new IdentifierSymbolEntry(TypeSystem::intType, "getint", identifiers->getLevel());
    SymbolEntry *se2 = new IdentifierSymbolEntry(TypeSystem::intType, "getch", identifiers->getLevel());
    SymbolEntry *se3 = new IdentifierSymbolEntry(TypeSystem::voidType, "putint", identifiers->getLevel());
    SymbolEntry *se4 = new IdentifierSymbolEntry(TypeSystem::voidType, "putch", identifiers->getLevel());
    identifiers->install("getint", se1);
    identifiers->install("getch", se2);
    identifiers->install("putint", se3);
    identifiers->install("putch", se4);
    yyparse();
    if(dump_ast)
        ast.output();
    return 0;
}
