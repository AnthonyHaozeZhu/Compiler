#include "Ast.h"
#include "SymbolTable.h"
#include "Unit.h"
#include "Instruction.h"
#include "IRBuilder.h"
#include <string>
#include "Type.h"

extern FILE *yyout;
int Node::counter = 0;
IRBuilder* Node::builder = nullptr;

Node::Node()
{
    seq = counter++;
}

void Node::backPatch(std::vector<Instruction*> &list, BasicBlock*bb)
{
    for(auto &inst:list)
    {
        if(inst->isCond())
            dynamic_cast<CondBrInstruction*>(inst)->setTrueBranch(bb);
        else if(inst->isUncond())
            dynamic_cast<UncondBrInstruction*>(inst)->setBranch(bb);
    }
}

std::vector<Instruction*> Node::merge(std::vector<Instruction*> &list1, std::vector<Instruction*> &list2)
{
    std::vector<Instruction*> res(list1);
    res.insert(res.end(), list2.begin(), list2.end());
    return res;
}

void Ast::genCode(Unit *unit)
{
    IRBuilder *builder = new IRBuilder(unit);
    Node::setIRBuilder(builder);
    root->genCode();
}

void FunctionDef::genCode()
{
    Unit *unit = builder->getUnit();
    Function *func = new Function(unit, se);
    BasicBlock *entry = func->getEntry();
    // set the insert point to the entry basicblock of this function.
    builder->setInsertBB(entry);

    stmt->genCode();

    /**
     * Construct control flow graph. You need do set successors and predecessors for each basic block.
     * Todo
    */
   
}

void BinaryExpr::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    if (op == AND)
    {
        BasicBlock *trueBB = new BasicBlock(func);  // if the result of lhs is true, jump to the trueBB.
        expr1->genCode();
        backPatch(expr1->trueList(), trueBB);
        builder->setInsertBB(trueBB);               // set the insert point to the trueBB so that intructions generated by expr2 will be inserted into it.
        expr2->genCode();
        true_list = expr2->trueList();
        false_list = merge(expr1->falseList(), expr2->falseList());
    }
    else if(op == OR)
    {
        // Todo
    }
    else if(op >= LESS && op <= GREATER)
    {
        // Todo
    }
    else if(op >= ADD && op <= SUB)
    {
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
        switch (op)
        {
        case ADD:
            opcode = BinaryInstruction::ADD;
            break;
        case SUB:
            opcode = BinaryInstruction::SUB;
            break;
        }
        new BinaryInstruction(opcode, dst, src1, src2, bb);
    }
}

void Constant::genCode()
{
    // we don't need to generate code.
}

void Id::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getAddr();
    new LoadInstruction(dst, addr, bb);
}

void IfStmt::genCode()
{
    Function *func;
    BasicBlock *then_bb, *end_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    cond->genCode();
    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), end_bb);

    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    builder->setInsertBB(end_bb);
}

void IfElseStmt::genCode()
{
    // Todo
}

void CompoundStmt::genCode()
{
    // Todo
}

void SeqNode::genCode()
{
    // Todo
}

// void DeclStmt::genCode()
// {
//     IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(ids->getSymPtr());
//     if(se->isGlobal())
//     {
//         Operand *addr;
//         SymbolEntry *addr_se;
//         addr_se = new IdentifierSymbolEntry(*se);
//         addr_se->setType(new PointerType(se->getType()));
//         addr = new Operand(addr_se);
//         se->setAddr(addr);
//     }
//     else if(se->isLocal())
//     {
//         Function *func = builder->getInsertBB()->getParent();
//         BasicBlock *entry = func->getEntry();
//         Instruction *alloca;
//         Operand *addr;
//         SymbolEntry *addr_se;
//         Type *type;
//         type = new PointerType(se->getType());
//         addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
//         addr = new Operand(addr_se);
//         alloca = new AllocaInstruction(addr, se);                   // allocate space for local id in function stack.
//         entry->insertFront(alloca);                                 // allocate instructions should be inserted into the begin of the entry block.
//         se->setAddr(addr);                                          // set the addr operand in symbol entry so that we can use it in subsequent code generation.
//     }
// }

void ReturnStmt::genCode()
{
    // Todo
}

void AssignStmt::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    expr->genCode();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(lval->getSymPtr())->getAddr();
    Operand *src = expr->getOperand();
    /***
     * We haven't implemented array yet, the lval can only be ID. So we just store the result of the `expr` to the addr of the id.
     * If you want to implement array, you have to caculate the address first and then store the result into it.
     */
    new StoreInstruction(addr, src, bb);
}

void SignleStmt::genCode()
{

}

void FuncRParams::genCode()
{
    
}

void Empty::genCode()
{
    
}

void FuncFParam::genCode()
{
    
}

void FuncFParams::genCode()
{
    
}

void ConstIdList::genCode()
{
    
}

void IdList::genCode()
{
    
}

void WhileStmt::genCode()
{
    
}

void FunctionCall::genCode()
{
    
}

void ConstDeclStmt::genCode()
{
    
}

void DeclStmt::genCode()
{
    
}

void ContinueStmt::genCode()
{
    
}

void BreakStmt::genCode()
{
    
}

void ConstId::genCode()
{
    
}

void SignleExpr::genCode()
{
    
}

void Ast::typeCheck()
{
    if(root != nullptr)
        root->typeCheck();
}

void FunctionDef::typeCheck()
{
    // Todo
}

void BinaryExpr::typeCheck()
{
    // Todo
}

void Constant::typeCheck()
{
    // Todo
}

void Id::typeCheck()
{
    // Todo
}

void IfStmt::typeCheck()
{
    // Todo
}

void IfElseStmt::typeCheck()
{
    // Todo
}

void CompoundStmt::typeCheck()
{
    // Todo
}

void SeqNode::typeCheck()
{
    // Todo
}

void DeclStmt::typeCheck()
{
    // Todo
}

void ReturnStmt::typeCheck()
{
    // Todo
}

void AssignStmt::typeCheck()
{
    // Todo
}


void SignleStmt::typeCheck()
{

}

void FuncRParams::typeCheck()
{
    
}

void Empty::typeCheck()
{
    
}

void FuncFParam::typeCheck()
{
    
}

void FuncFParams::typeCheck()
{
    
}

void ConstIdList::typeCheck()
{
    
}

void IdList::typeCheck()
{
    
}

void WhileStmt::typeCheck()
{
    
}

void FunctionCall::typeCheck()
{
    
}

void ConstDeclStmt::typeCheck()
{
    
}

void ContinueStmt::typeCheck()
{
    
}

void BreakStmt::typeCheck()
{
    
}

void ConstId::typeCheck()
{
    
}

void SignleExpr::typeCheck()
{
    
}


void Ast::output()
{
    fprintf(yyout, "program\n");
    if(root != nullptr)
        root->output(4);
}

void BinaryExpr::output(int level)
{
    std::string op_str;
    switch(op)
    {
        case ADD:
            op_str = "add";
            break;
        case SUB:
            op_str = "sub";
            break;
        case AND:
            op_str = "and";
            break;
        case OR:
            op_str = "or";
            break;
        case LESS:
            op_str = "less";
            break;
        case MORE:
            op_str = "more";
            break;
        case MOREEQUAL:
            op_str = "moreequal";
            break;
        case LESSEQUAL:
            op_str = "lessequal";
            break;
        case EQUAL:
            op_str = "equal";
            break;
        case NOEQUAL:
            op_str = "noequal";
            break;
        case DIV:
            op_str = "div";
            break;
        case MUL:
            op_str = "mul";
            break;
        case PERC:
            op_str = "mod";
            break;
    }
    fprintf(yyout, "%*cBinaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr1->output(level + 4);
    expr2->output(level + 4);
}

void SignleExpr::output(int level)
{
    std::string op_str;
    switch(op)
    {
        case SUB:
            op_str = "negative";
            break;
        case ADD:
            op_str = "positive";
            break;
        case EXCLAMATION:
            op_str = "anti";
            break;
    }
    fprintf(yyout, "%*cSignleExpr\top: %s\n", level, ' ', op_str.c_str());
    expr->output(level + 4);
}


void Constant::output(int level)
{
    std::string type, value;
    type = symbolEntry->getType()->toStr();
    value = symbolEntry->toStr();
    fprintf(yyout, "%*cIntegerLiteral\tvalue: %s\ttype: %s\n", level, ' ',
            value.c_str(), type.c_str());
}

void ConstId::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    fprintf(yyout, "%*cConstId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}

void Id::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}

void FuncFParam::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry -> toStr();
    type = symbolEntry -> getType() -> toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry) -> getScope();
    fprintf(yyout, "%*cFuncFParam\tname:%s\tscope:%d\ttype:%s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}

void CompoundStmt::output(int level)
{
    fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
    stmt->output(level + 4);
}

void SeqNode::output(int level)
{
    fprintf(yyout, "%*cSequence\n", level, ' ');
    stmt1->output(level + 4);
    stmt2->output(level + 4);
}

void BreakStmt::output(int level)
{
    fprintf(yyout, "%*cBreakStmt\n", level, ' ');
}

void ContinueStmt::output(int level)
{
    fprintf(yyout, "%*cContinueStmt\n", level, ' ');
}

void DeclStmt::output(int level)
{
    fprintf(yyout, "%*cDeclStmt\n", level, ' ');
    ids->output(level + 4);
}

void ConstDeclStmt::output(int level)
{
    fprintf(yyout, "%*cConstDeclStmt\n", level, ' ');
    Cids->output(level + 4);
}

void IfStmt::output(int level)
{
    fprintf(yyout, "%*cIfStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
}

void IfElseStmt::output(int level)
{
    fprintf(yyout, "%*cIfElseStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
    elseStmt->output(level + 4);
}

void ReturnStmt::output(int level)
{
    fprintf(yyout, "%*cReturnStmt\n", level, ' ');
    retValue->output(level + 4);
}

void AssignStmt::output(int level)
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);
    expr->output(level + 4);
}

void FunctionDef::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFunctionDefine function name: %s, type: %s\n", level, ' ', 
            name.c_str(), type.c_str());
    if(FPs != nullptr){
        FPs -> output(level + 4);
    }
    stmt->output(level + 4);
}

void FunctionCall::output(int level)
{
    std::string name, type;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    fprintf(yyout, "%*cFuncCall\tname: %s\ttype: %s\n", level, ' ',
            name.c_str(), type.c_str());
    if(RPs != nullptr)
    {
        RPs -> output(level + 4);
    }
}

void WhileStmt::output(int level)
{
    fprintf(yyout, "%*cWhileStmt\n", level, ' ');
    cond->output(level + 4);
    loop->output(level + 4);
}

void IdList::output(int level)
{
    fprintf(yyout, "%*cIdList\n", level, ' ');
    for(long unsigned int i = 0; i < Ids.size(); i++)
    {
        Ids[i] -> output(level + 4);
    }
    for(long unsigned int i = 0; i < Assigns.size(); i++)
    {
        Assigns[i] -> output(level + 4);
    }
}
void ConstIdList::output(int level)
{
    fprintf(yyout, "%*cConstIdList\n", level, ' ');
    for(long unsigned int i = 0; i < CIds.size(); i++)
    {
        CIds[i] -> output(level + 4);
        Assigns[i] -> output(level + 4);
    }
}

void FuncFParams::output(int level)
{
    fprintf(yyout, "%*cFuncFParams\n", level, ' ');
    for(long unsigned int i = 0; i < FPs.size(); i++)
    {
        FPs[i] -> output(level + 4);
    }
    for(long unsigned int i = 0; i < Assigns.size(); i++)
    {
        Assigns[i] -> output(level + 4);
    }
}

void FuncRParams::output(int level)
{
    fprintf(yyout, "%*cFuncRParams\n", level, ' ');
    for(long unsigned int i = 0; i < Exprs.size(); i++)
    {
        Exprs[i] -> output(level + 4);
    }
}

void Empty::output(int level)
{
    fprintf(yyout, "%*cEmpty Statement\n", level, ' ');
}

void SignleStmt::output(int level)
{
    fprintf(yyout, "%*cSignle Statement\n", level, ' ');
    expr -> output(level + 4);
}