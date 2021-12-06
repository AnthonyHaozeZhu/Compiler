#include "Function.h"
#include "Unit.h"
#include "Type.h"
#include <list>

extern FILE* yyout;

Function::Function(Unit *u, SymbolEntry *s)
{
    u->insertFunc(this);
    entry = new BasicBlock(this);
    sym_ptr = s;
    parent = u;
}

Function::~Function()
{
    auto delete_list = block_list;
    for (auto &i : delete_list)
        delete i;
    parent->removeFunc(this);
}

// remove the basicblock bb from its block_list.
void Function::remove(BasicBlock *bb)
{
    block_list.erase(std::find(block_list.begin(), block_list.end(), bb));
}

void Function::output() const
{
    FunctionType* funcType = dynamic_cast<FunctionType*>(sym_ptr->getType());
    Type *retType = funcType->getRetType();
    fprintf(yyout, "define %s %s() {\n", retType->toStr().c_str(), sym_ptr->toStr().c_str());
//    std::cout << "执行了Function的define" << std::endl;
    std::set<BasicBlock *> v;
    std::list<BasicBlock *> q;
    q.push_back(entry);
    v.insert(entry);
    while (!q.empty())
    {
//        std::cout << "进入了Function的while循环" << std::endl;
        auto bb = q.front();
        q.pop_front();
        bb->output();//出现问题的地方
        for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)
        {
//            std::cout << "进入了function的for循环" << std::endl;
            if (v.find(*succ) == v.end())
            {
                v.insert(*succ);
                q.push_back(*succ);
//                std::cout << "function的if判断完毕" << std::endl;
            }
        }
    }
    fprintf(yyout, "}\n");
}
