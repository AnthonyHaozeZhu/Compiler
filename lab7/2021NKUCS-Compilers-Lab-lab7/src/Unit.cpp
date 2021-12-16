#include "SymbolTable.h"
#include "Type.h"
#include "Unit.h"

extern FILE* yyout;
void Unit::insertFunc(Function *f)
{
    func_list.push_back(f);
}

void Unit::removeFunc(Function *func)
{
    func_list.erase(std::find(func_list.begin(), func_list.end(), func));
}

void Unit::outputDecl() const
{
    for(auto &var:SymbolTable::globals->getTable())
    {
        IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry*>(var.second);
        if(se->getType()->isFunc())
        {
            if(se->isExtern())
            {
                FunctionType *funcType = dynamic_cast<FunctionType*>(se->getType());
                Type *retType = funcType->getRetType();
                std::string retStr = retType->toStr();
                std::string paramStr = funcType->paramsToStr();
                std::string name = se->toStr();
                fprintf(yyout, "declare %s %s%s\n", retStr.c_str(), name.c_str(), paramStr.c_str());
            }
            continue;
        }
        std::string name, type, val;
        name = se->toStr();
        type = se->getType()->toStr();
        val = se->getInitValStr();
        fprintf(yyout, "%s = global %s %s, align 4\n", name.c_str(), type.c_str(), val.c_str());
    }
}

void Unit::genMachineCode(MachineUnit* munit) 
{
    AsmBuilder* builder = new AsmBuilder();
    builder->setUnit(munit);
    for (auto &func : func_list)
        func->genMachineCode(builder);
}

void Unit::output() const
{
    outputDecl();
    for (auto &func : func_list)
        func->output();
}

Unit::~Unit()
{
    for(auto &func:func_list)
        delete func;
}


