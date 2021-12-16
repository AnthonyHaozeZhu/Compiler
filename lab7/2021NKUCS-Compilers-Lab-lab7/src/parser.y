%code top{
    #include <iostream>
    #include <assert.h>
    #include "parser.h"
    extern Ast ast;
    int yylex();
    int yyerror( char const * );
    Type *global_type;
}

%code requires {
    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"
}

%union {
    int itype;
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    Node* nodetype;
    Type* type;
}

%start Program
%token <strtype> ID 
%token <itype> INTEGER
%token IF ELSE WHILE
%token INT VOID
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON LBRACKET RBRACKET COMMA
%token ADD ASSIGN EQUAL NOT SUB MUL DIV MOD OR AND NOTEQUAL LESS GREATER LESSEQUAL GREATEREQUAL
%token RETURN CONST

%nterm <stmttype> Stmts Stmt AssignStmt BlockStmt IfStmt ReturnStmt DeclStmt FuncDef ExprStmt
%nterm <stmttype> BlankStmt WhileStmt ConstDecl ConstDefList ConstDef VarDecl VarDefList VarDef
%nterm <exprtype> Exp AddExp MulExp EqExp Cond LOrExp PrimaryExp LVal RelExp LAndExp UnaryExp 
%nterm <exprtype> ConstExp FuncRParams OptFuncFParams  FuncFParams FuncFParam
%nterm <type> Type

%precedence THEN
%precedence ELSE
%%
Program
    : Stmts {
        ast.setRoot($1);
    }
    ;
Stmts
    : Stmt {$$=$1;}
    | Stmts Stmt{
        $$ = new SeqNode($1, $2);
    }
    ;
Stmt
    : AssignStmt {$$=$1;}
    | BlockStmt {$$=$1;}
    | IfStmt {$$=$1;}
    | ReturnStmt {$$=$1;}
    | DeclStmt {$$=$1;}
    | FuncDef {$$=$1;}
    | ExprStmt {$$=$1;}
    | BlankStmt {$$=$1;}
    | WhileStmt {$$=$1;}
    ;
LVal
    : ID {
        SymbolEntry *se;
        se = SymbolTable::identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$ = new Id(se);
        delete []$1;
    }
    ;
AssignStmt
    :
    LVal ASSIGN Exp SEMICOLON {
        $$ = new AssignStmt($1, $3);
    }
    ;
ExprStmt
    : Exp SEMICOLON {
        $$ = new ExprStmt($1);
    }
    ;
BlankStmt
    : SEMICOLON {
        $$ = new BlankStmt();
    }
    ;
BlockStmt
    :   LBRACE 
        {SymbolTable::identifiers = new SymbolTable(SymbolTable::identifiers);} 
        Stmts RBRACE 
        {
            $$ = new CompoundStmt($3);
            SymbolTable *top = SymbolTable::identifiers;
            SymbolTable::identifiers = SymbolTable::identifiers->getPrev();
            delete top;
        }
    | LBRACE RBRACE
    {
        $$ = new BlankStmt();
    }
    ;
IfStmt
    : IF LPAREN Cond RPAREN Stmt %prec THEN {
        $$ = new IfStmt($3, $5);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        $$ = new IfElseStmt($3, $5, $7);
    }
    ;
WhileStmt
    : WHILE LPAREN Cond RPAREN Stmt {
        $$ = new WhileStmt($3, $5);
    }
    ;
ReturnStmt
    :
    RETURN Exp SEMICOLON{
        $$ = new ReturnStmt($2);
    }
    ;
Exp
    :
    AddExp {$$ = $1;}
    ;
Cond
    :
    LOrExp {$$ = $1;}
    ;
PrimaryExp
    :
    LPAREN Exp RPAREN {
        $$ = $2;
    }
    |
    LVal {
        $$ = $1;
    }
    | INTEGER {
        SymbolEntry *se = new ConstantSymbolEntry(IntType::get(32), $1);
        $$ = new Constant(se);
    }
    | ID LPAREN FuncRParams RPAREN {
        SymbolEntry *t;
        t = SymbolTable::identifiers->lookup($1);
        assert(t != nullptr);
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(32), SymbolTable::getLabel());
        $$ = new CallExpr(se, t, $3);
        delete []$1;
    }
    | ID LPAREN RPAREN {
        SymbolEntry *t;
        t = SymbolTable::identifiers->lookup($1);
        assert(t != nullptr);
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(32), SymbolTable::getLabel());
        $$ = new CallExpr(se, t, nullptr);
        delete []$1;
    }
    ;
UnaryExp
    :
    PrimaryExp {$$ = $1;}
    |
    ADD UnaryExp{$$ = $2;}
    |
    NOT UnaryExp {
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(32), SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::NOT, $2);
    }
    |
    SUB UnaryExp{
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(32), SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::UMINUS, $2);
    }
    ;
MulExp
    :
    UnaryExp {$$ = $1;}
    |
    MulExp MUL UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(32), SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MUL, $1, $3);
    }
    |
    MulExp DIV UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(32), SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::DIV, $1, $3);
    }
    |
    MulExp MOD UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(32), SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MOD, $1, $3);
    }
    ;
AddExp
    :
    MulExp {$$ = $1;}
    |
    AddExp ADD MulExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(32), SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
    }
    |
    AddExp SUB MulExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(32), SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
    }
    ;
RelExp
    :
    AddExp {$$ = $1;}
    |
    RelExp LESS AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(1), SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::L, $1, $3);
    }
    |
    RelExp GREATER AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(1), SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::G, $1, $3);
    }
    |
    RelExp LESSEQUAL AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(1), SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LE, $1, $3);
    }
    |
    RelExp GREATEREQUAL AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(1), SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GE, $1, $3);
    }
    ;
EqExp
    :
    RelExp {$$=$1;}
    |
    EqExp EQUAL RelExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(1), SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::EQ, $1, $3);
    }
    |
    EqExp NOTEQUAL RelExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(1), SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::NE, $1, $3);
    }
    ;
LAndExp
    :
    EqExp {$$ = $1;}
    |
    LAndExp AND EqExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(1), SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    }
    ;
LOrExp
    :
    LAndExp {$$ = $1;}
    |
    LOrExp OR LAndExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(IntType::get(1), SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::OR, $1, $3);
    }
    ;
ConstExp
    :
    AddExp {
        $$ = $1;
        }
    ;
FuncRParams
    :
    Exp {$$ = $1;}
    |
    FuncRParams COMMA Exp {
        $$ = new FuncRParams($1, $3);
    }
    ;
Type
    : INT {
        $$ = global_type = IntType::get(32);
        
    }
    | VOID {
        $$ = global_type = VoidType::get();
    }
    ;
DeclStmt
    :
    ConstDecl {$$=$1;}
    |
    VarDecl {$$=$1;}
    ;
ConstDecl
    :
    CONST Type ConstDefList SEMICOLON {
        $$ = $3;
    }
    ;
ConstDefList
    :
    ConstDef{$$=$1;}
    |
    ConstDefList COMMA ConstDef
    {
        $$ = new DefList($1, $3);
    }
    ;
ConstDef
    :
    ID ASSIGN ConstExp {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(global_type, $1, SymbolTable::identifiers->getLevel());
        SymbolTable::identifiers->install($1, se);
        $$ = new ConstDecl(new Id(se), $3);
        delete []$1;
    }
    ;
VarDecl
    :
    Type VarDefList SEMICOLON {
        $$ = $2;
    }
    ;
VarDefList
    :
    VarDefList COMMA VarDef {
        $$ = new DefList($1, $3);
    }
    |
    VarDef{
        $$ = $1;
    }
    ;
VarDef
    :
    ID {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(global_type, $1, SymbolTable::identifiers->getLevel());
        SymbolTable::identifiers->install($1, se);
        $$ = new DeclStmt(new Id(se));
        delete []$1;
    }
    |
    ID ASSIGN Exp {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(global_type, $1, SymbolTable::identifiers->getLevel());
        SymbolTable::identifiers->install($1, se);
        $$ = new DeclStmt(new Id(se), $3);
        delete []$1;
    }
    ;
FuncDef
    :
    Type ID {
        SymbolEntry *se = new IdentifierSymbolEntry(nullptr, $2, SymbolTable::identifiers->getLevel());
        SymbolTable::identifiers->install($2, se);
        SymbolTable::identifiers = new SymbolTable(SymbolTable::identifiers);
    }
    LPAREN OptFuncFParams RPAREN
    BlockStmt
    {
        SymbolEntry *se;
        se = SymbolTable::identifiers->lookup($2);
        assert(se != nullptr);
        Type *retType = $1;
        std::vector<Type*> paramsType;
        if($5 != nullptr)
            $5->getTypes(paramsType);
        Type *funcType = FunctionType::get(retType, paramsType);
        se->setType(funcType);
        $$ = new FunctionDef(se, $5, $7);
        SymbolTable *top = SymbolTable::identifiers;
        SymbolTable::identifiers = SymbolTable::identifiers->getPrev();
        delete top;
        delete []$2;
    }
    ;
OptFuncFParams
    :
    FuncFParams {$$=$1;}
    | %empty {$$ = nullptr;}
    ;
FuncFParams
    :
    FuncFParams COMMA FuncFParam {
        $$ = new FuncFParams($1, $3);
    }
    |
    FuncFParam {$$ = $1;}
    ;
FuncFParam
    :
    Type ID {
        SymbolEntry *se = new IdentifierSymbolEntry($1, $2, SymbolTable::identifiers->getLevel());
        SymbolTable::identifiers->install($2, se);
        $$ = new FuncFParam(se);
    }
    ;
%%

int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}
