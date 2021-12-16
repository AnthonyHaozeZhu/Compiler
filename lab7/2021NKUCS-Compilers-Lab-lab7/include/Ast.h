#ifndef __AST_H__
#define __AST_H__

#include <fstream>
#include "Operand.h"

class SymbolEntry;
class Unit;
class Function;
class BasicBlock;
class Instruction;
class IRBuilder;

class Node
{
private:
    static int counter;
    int seq;
protected:
    std::vector<BasicBlock**> true_list;
    std::vector<BasicBlock**> false_list;
    static IRBuilder *builder;
    void backPatch(std::vector<BasicBlock**> &list, BasicBlock*target);
    std::vector<BasicBlock**> merge(std::vector<BasicBlock**> &list1, std::vector<BasicBlock**> &list2);

public:
    Node();
    int getSeq() const {return seq;};
    static void setIRBuilder(IRBuilder*ib) {builder = ib;};
    virtual void output(int level) = 0;
    virtual void typeCheck() = 0;
    virtual void genCode() = 0;
    std::vector<BasicBlock**>& trueList() {return true_list;}
    std::vector<BasicBlock**>& falseList() {return false_list;}
};

class ExprNode : public Node
{
protected:
    SymbolEntry *symbolEntry;
    Operand *dst;   // The result of the subtree is stored into dst.
public:
    ExprNode(SymbolEntry *symbolEntry) : symbolEntry(symbolEntry){};
    Operand* getOperand() {return dst;};
    SymbolEntry* getSymPtr() {return symbolEntry;};
    virtual void getOperands(std::vector<Operand*> &operands){operands.push_back(dst);};
    virtual void getTypes(std::vector<Type*> &types){types.push_back(symbolEntry->getType());};
    virtual void getSyms(std::vector<SymbolEntry*> &ses){ses.push_back(symbolEntry);};
    virtual bool constantFolding(int &val){return false;};
};

class Extend : public ExprNode
{
private:
    ExprNode *expr;
public:
    Extend(SymbolEntry *se, ExprNode*expr) : ExprNode(se), expr(expr) {dst = new Operand(se);};
    void output(int level) {};
    void typeCheck() {expr->typeCheck();};
    void genCode();
};

class BinaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {ADD, SUB, MUL, DIV, MOD, AND, OR, L, G, LE, GE, EQ, NE};
    BinaryExpr(SymbolEntry *se, int op, ExprNode*expr1, ExprNode*expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2){dst = new Operand(se);};
    void output(int level);
    void typeCheck();
    void genCode();
    bool constantFolding(int &val);
};

class UnaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr;
public:
    enum {UMINUS, NOT};
    UnaryExpr(SymbolEntry *se, int op, ExprNode*expr) : ExprNode(se), op(op), expr(expr){dst = new Operand(se);};
    void output(int level){};
    void typeCheck();
    void genCode();
    bool constantFolding(int &val);
};

class FuncRParams : public ExprNode
{
private:
    ExprNode *params;
    ExprNode *param;
public:
    FuncRParams(ExprNode *params, ExprNode *expr) : ExprNode(nullptr), params(params), param(expr) {};
    void output(int level){};
    void typeCheck();
    void genCode();
    void getOperands(std::vector<Operand*> &operands);
};

class Constant : public ExprNode
{
public:
    Constant(SymbolEntry *se) : ExprNode(se){dst = new Operand(se);};
    void output(int level);
    void typeCheck();
    void genCode();
    bool constantFolding(int &val);
};

class Id : public ExprNode
{
public:
    Id(SymbolEntry *se) : ExprNode(se){SymbolEntry *temp = new TemporarySymbolEntry(se->getType(), SymbolTable::getLabel()); dst = new Operand(temp);};
    void output(int level);
    void typeCheck();
    void genCode();
    bool constantFolding(int &val);
};

class CallExpr : public ExprNode
{
private:
    SymbolEntry* callee;
    ExprNode* params;
public:
    CallExpr(SymbolEntry *se, SymbolEntry *t, ExprNode *params) : ExprNode(se), callee(t), params(params) {dst = new Operand(se);};
    void output(int level){};
    void typeCheck();
    void genCode();
};


class FuncFParams : public ExprNode
{
private:
    ExprNode *params;
    ExprNode *param;
public:
    FuncFParams(ExprNode *params, ExprNode *param) : ExprNode(nullptr), params(params), param(param) {};
    void output(int level){};
    void typeCheck(){};
    void genCode();
    void getTypes(std::vector<Type*>&);
    void getSyms(std::vector<SymbolEntry*> &ses);
};

class FuncFParam : public ExprNode
{
public:
    FuncFParam(SymbolEntry *se) : ExprNode(se) {};
    void output(int level){};
    void typeCheck(){};
    void genCode();
};

class StmtNode : public Node
{};

class ExprStmt : public StmtNode
{
private:
    ExprNode *expr;
public:
    ExprStmt(ExprNode *expr) : expr(expr) {};
    void output(int level){};
    void typeCheck(){expr->typeCheck();};
    void genCode() {expr->genCode();};
};

class BlankStmt : public StmtNode
{
public:
    void output(int level){};
    void typeCheck(){};
    void genCode(){};
};

class CompoundStmt : public StmtNode
{
private:
    StmtNode *stmt;
public:
    CompoundStmt(StmtNode *stmt) : stmt(stmt) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class SeqNode : public StmtNode
{
private:
    StmtNode *stmt1, *stmt2;
public:
    SeqNode(StmtNode *stmt1, StmtNode *stmt2) : stmt1(stmt1), stmt2(stmt2){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class DeclStmt : public StmtNode
{
private:
    Id *id;
    ExprNode *initVal;
public:
    DeclStmt(Id *id) : id(id){initVal = nullptr;};
    DeclStmt(Id *id, ExprNode *initVal) : id(id), initVal(initVal){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ConstDecl : public StmtNode
{
private:
    Id *id;
    ExprNode *initVal;
public:
    ConstDecl(Id *id, ExprNode *initVal) : id(id), initVal(initVal){};
    void output(int level){};
    void typeCheck();
    void genCode();
};

class DefList : public StmtNode
{
private:
    StmtNode *def1;
    StmtNode *def2;
public:
    DefList(StmtNode*stmt1, StmtNode*stmt2) : def1(stmt1), def2(stmt2) {};
    void output(int level){};
    void typeCheck(){def1->typeCheck();def2->typeCheck();};
    void genCode();
};

class IfStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
public:
    IfStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class IfElseStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
    StmtNode *elseStmt;
public:
    IfElseStmt(ExprNode *cond, StmtNode *thenStmt, StmtNode *elseStmt) : cond(cond), thenStmt(thenStmt), elseStmt(elseStmt) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class WhileStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *body;
public:
    WhileStmt(ExprNode *cond, StmtNode *bodyStmt) : cond(cond), body(bodyStmt){};
    void output(int level){};
    void typeCheck();
    void genCode();
};

class ReturnStmt : public StmtNode
{
private:
    ExprNode *retValue;
public:
    ReturnStmt(ExprNode*retValue) : retValue(retValue) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class AssignStmt : public StmtNode
{
private:
    ExprNode *lval;
    ExprNode *expr;
public:
    AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class FunctionDef : public StmtNode
{
private:
    SymbolEntry *se;
    ExprNode *params;
    StmtNode *stmt;
public:
    FunctionDef(SymbolEntry *se, ExprNode *params, StmtNode *stmt) : se(se), params(params), stmt(stmt){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class Ast
{
private:
    Node* root;
public:
    Ast() {root = nullptr;}
    void setRoot(Node*n) {root = n;}
    void output();
    void typeCheck();
    void genCode(Unit *unit);
};

#endif
