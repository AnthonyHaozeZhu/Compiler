#include "Unit.h"
#include <algorithm>
#include <stack>
#include <string>
#include "Ast.h"
#include "SymbolTable.h"
#include "Type.h"
extern FILE* yyout;

void Unit::insertFunc(Function* f) {
    func_list.push_back(f);
}

void Unit::removeFunc(Function* func) {
    func_list.erase(std::find(func_list.begin(), func_list.end(), func));
}

void Unit::insertGlobal(SymbolEntry* se) {
    global_list.push_back(se);
}

void Unit::insertDeclare(SymbolEntry* se) {
    auto it = std::find(declare_list.begin(), declare_list.end(), se);
    if (it == declare_list.end()) {
        declare_list.push_back(se);
    }
}

void Unit::output() const {
    for (auto se : global_list) {
        if (se->getType()->isInt())
            fprintf(yyout, "%s = global %s %d, align 4\n", se->toStr().c_str(),
                    se->getType()->toStr().c_str(),
                    ((IdentifierSymbolEntry*)se)->getValue());
        else if (se->getType()->isArray()) {
            ArrayType* type = (ArrayType*)(se->getType());
            // int size = type->getSize() / TypeSystem::intType->getSize();
            int* val = ((IdentifierSymbolEntry*)se)->getArrayValue();
            int i = 0;
            fprintf(yyout, "%s = global ", se->toStr().c_str());
            if (((IdentifierSymbolEntry*)se)->isAllZero()) {
                fprintf(yyout, "%s zeroinitializer", type->toStr().c_str());
            } else {
                std::stack<ArrayType*> stk;
                std::stack<int> stk1;
                stk.push(type);
                stk1.push(0);
                ArrayType* temp;
                while (!stk.empty()) {
                    temp = stk.top();
                    if (temp->getElementType()->isInt()) {
                        fprintf(yyout, "%s [", temp->toStr().c_str());
                        for (int j = 0; j < temp->getLength(); j++) {
                            if (j != 0)
                                fprintf(yyout, ", ");
                            fprintf(yyout, "i32 %d", val[i++]);
                        }
                        fprintf(yyout, "]");
                        stk1.pop();
                        stk.pop();
                        if (stk.empty())
                            break;
                        stk1.top()++;
                        continue;
                    }
                    if (stk1.top() != temp->getLength()) {
                        stk.push((ArrayType*)(temp->getElementType()));
                        if (stk1.top() == 0)
                            fprintf(yyout, "%s [", temp->toStr().c_str());
                        if (stk1.top() != 0)
                            fprintf(yyout, ", ");
                        stk1.push(0);
                    } else {
                        fprintf(yyout, "]");
                        stk.pop();
                        stk1.pop();
                        if (stk.empty())
                            break;
                        stk1.top()++;
                        continue;
                    }
                }
            }
            fprintf(yyout, ", align 4\n");
        }
    }

    for (auto& func : func_list)
        func->output();
    for (auto se : declare_list) {
        FunctionType* type = (FunctionType*)(se->getType());
        std::string str = type->toStr();
        std::string name = str.substr(0, str.find('('));
        std::string param = str.substr(str.find('('));
        fprintf(yyout, "declare %s %s%s\n", type->getRetType()->toStr().c_str(),
                se->toStr().c_str(), param.c_str());
    }
}

void Unit::genMachineCode(MachineUnit* munit) 
{
    AsmBuilder* builder = new AsmBuilder();
    builder->setUnit(munit);
    for (auto &func : func_list)
        func->genMachineCode(builder);
}

Unit::~Unit()
{
    for(auto &func:func_list)
        delete func;
}


