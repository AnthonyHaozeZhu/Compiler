#include "MachineCode.h"
#include <iostream>
#include "Type.h"
extern FILE* yyout;

void MachineBlock::output() 
{
    int offset = (parent->getSavedRegs().size() + 2) * 4;
    int num = parent->getParamsNum();
    int count = 0;
    if (!inst_list.empty()) 
    {
        fprintf(yyout, ".L%d:\n", this->no);
        for (long unsigned int i = 0; i < inst_list.size(); i++) 
        {
            if (num > 4 && (inst_list[i])->isStore()) 
            {
                MachineOperand* operand = (inst_list[i])->getUse()[0];
                if (operand->isReg() && operand->getReg() == 0) 
                {
                    if (count != 0) 
                    {
                        count++;
                    } 
                    else 
                    {
                        offset += 4;
                        LoadMInstruction *cur_inst = new LoadMInstruction(this, new MachineOperand(MachineOperand::REG, 0), new MachineOperand(MachineOperand::REG, 11), new MachineOperand(MachineOperand::IMM, offset));
                        cur_inst->output();
                    }
                }
            }
            if ((inst_list[i])->isBX()) 
            {
                auto cur_inst = new StackMInstrcuton(this, StackMInstrcuton::POP, parent->getSavedRegs(), new MachineOperand(MachineOperand::REG, 11), new MachineOperand(MachineOperand::REG, 14));
                cur_inst->output();
            }
            if ((inst_list[i])->isAdd()) 
            {
                auto dst = inst_list[i]->getDef()[0];
                auto src1 = inst_list[i]->getUse()[0];
                if (dst->isReg() && dst->getReg() == 13 && src1->isReg() && src1->getReg() == 13 && (inst_list[i + 1])->isBX()) 
                {
                    int size = parent->AllocSpace(0);
                    (inst_list[i])->getUse()[1]->setVal(size);
                }
            }
            (inst_list[i])->output();
        }
    }
}

MachineOperand::MachineOperand(int tp, int val) 
{
    this->type = tp;
    if (tp == MachineOperand::IMM)
        this->val = val;
    else
        this->reg_no = val;   
}

MachineOperand::MachineOperand(std::string label) 
{
    this->type = MachineOperand::LABEL;
    this->label = label;
}

void MachineOperand::PrintReg() 
{
    switch (reg_no) 
    {
        case 11:
            fprintf(yyout, "fp");
            break;
        case 13:
            fprintf(yyout, "sp");
            break;
        case 14:
            fprintf(yyout, "lr");
            break;
        case 15:
            fprintf(yyout, "pc");
            break;
        default:
            fprintf(yyout, "r%d", reg_no);
            break;
    }
}

void MachineOperand::output() {
    /* HINT：print operand
     * Example:
     * immediate num 1 -> print #1;
     * register 1 -> print r1;
     * lable addr_a -> print addr_a; */
    switch (this->type) {
        case IMM:
            fprintf(yyout, "#%d", this->val);
            break;
        case VREG:
            fprintf(yyout, "v%d", this->reg_no);
            break;
        case REG:
            PrintReg();
            break;
        case LABEL:
            if (this->label.substr(0, 2) == ".L")
                fprintf(yyout, "%s", this->label.c_str());
            else if (this->label.substr(0, 1) == "@")
                fprintf(yyout, "%s", this->label.c_str() + 1);
            else
                fprintf(yyout, "addr_%s%d", this->label.c_str(), parent->getParent()->getParent()->getParent()->getGnumber());
        default:                  
            break;
    }
}

void MachineInstruction::PrintCond() {
    switch (cond) {
        case EQ:
            fprintf(yyout, "eq");
            break;
        case NE:
            fprintf(yyout, "ne");
            break;
        case LT:
            fprintf(yyout, "lt");
            break;
        case LE:
            fprintf(yyout, "le");
            break;
        case GT:
            fprintf(yyout, "gt");
            break;
        case GE:
            fprintf(yyout, "ge");
            break;
        default:
            break;
    }
}

void MachineInstruction::insertBefore(MachineInstruction* inst) 
{
    std::vector<MachineInstruction *> &instructions = parent->getInsts();
    std::vector<MachineInstruction *>::iterator it = std::find(instructions.begin(), instructions.end(), this);
    instructions.insert(it, inst);
}

void MachineInstruction::insertAfter(MachineInstruction* inst) 
{
    std::vector<MachineInstruction *> &instructions = parent->getInsts();
    std::vector<MachineInstruction *>::iterator it = std::find(instructions.begin(), instructions.end(), this);
    instructions.insert(++it, inst);
}

BinaryMInstruction::BinaryMInstruction(MachineBlock* p, int op, MachineOperand* dst, MachineOperand* src1,MachineOperand* src2,int cond) 
{
    this->parent = p;
    this->type = MachineInstruction::BINARY;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    src2->setParent(this);
}

void BinaryMInstruction::output() 
{
    switch (this->op) 
    {
        case BinaryMInstruction::ADD:
            fprintf(yyout, "\tadd ");
            this->def_list[0]->output();
            fprintf(yyout, ", ");
            this->use_list[0]->output();
            fprintf(yyout, ", ");
            this->use_list[1]->output();
            fprintf(yyout, "\n");
            break;
        case BinaryMInstruction::SUB:
            fprintf(yyout, "\tsub ");
            this->def_list[0]->output();
            fprintf(yyout, ", ");
            this->use_list[0]->output();
            fprintf(yyout, ", ");
            this->use_list[1]->output();
            fprintf(yyout, "\n");
            break;
        case BinaryMInstruction::AND:
            fprintf(yyout, "\tand ");
            this->def_list[0]->output();
            fprintf(yyout, ", ");
            this->use_list[0]->output();
            fprintf(yyout, ", ");
            this->use_list[1]->output();
            fprintf(yyout, "\n");
            break;
        case BinaryMInstruction::OR:
            fprintf(yyout, "\torr ");
            this->def_list[0]->output();
            fprintf(yyout, ", ");
            this->use_list[0]->output();
            fprintf(yyout, ", ");
            this->use_list[1]->output();
            fprintf(yyout, "\n");
            break;
        case BinaryMInstruction::MUL:
            fprintf(yyout, "\tmul ");
            this->def_list[0]->output();
            fprintf(yyout, ", ");
            this->use_list[0]->output();
            fprintf(yyout, ", ");
            this->use_list[1]->output();
            fprintf(yyout, "\n");
            break;
        case BinaryMInstruction::DIV:
            fprintf(yyout, "\tsdiv ");
            this->def_list[0]->output();
            fprintf(yyout, ", ");
            this->use_list[0]->output();
            fprintf(yyout, ", ");
            this->use_list[1]->output();
            fprintf(yyout, "\n");
            break;
        default:
            break;
    }
}

LoadMInstruction::LoadMInstruction(MachineBlock* p, MachineOperand* dst, MachineOperand* src1, MachineOperand* src2, int cond)
{
    this->parent = p;
    this->type = MachineInstruction::LOAD;
    this->op = -1;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    if (src2)
        this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    if (src2)
        src2->setParent(this);
}

void LoadMInstruction::output() 
{
    fprintf(yyout, "\tldr ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");

    if (this->use_list[0]->isImm()) 
    {
        fprintf(yyout, "=%d\n", this->use_list[0]->getVal());
        return;
    }

    // Load address
    if (this->use_list[0]->isReg() || this->use_list[0]->isVReg())
        fprintf(yyout, "[");

    this->use_list[0]->output();
    if (this->use_list.size() > 1) {
        fprintf(yyout, ", ");
        this->use_list[1]->output();
    }

    if (this->use_list[0]->isReg() || this->use_list[0]->isVReg())
        fprintf(yyout, "]");
    fprintf(yyout, "\n");
}

StoreMInstruction::StoreMInstruction(MachineBlock* p, MachineOperand* src1, MachineOperand* src2, MachineOperand* src3, int cond)
{
    this->parent = p;
    this->type = MachineInstruction::STORE;
    this->op = -1;
    this->cond = cond;
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    if (src3)
    {
        this->use_list.push_back(src3);
    }
    src1->setParent(this);
    src2->setParent(this);
    if (src3)
    {
        src3->setParent(this);
    }
}

void StoreMInstruction::output() 
{
    fprintf(yyout, "\tstr ");
    this->use_list[0]->output();
    fprintf(yyout, ", ");
    if (this->use_list[1]->isReg() || this->use_list[1]->isVReg())
    {
        fprintf(yyout, "[");
    }
    this->use_list[1]->output();
    if (this->use_list.size() > 2) 
    {
        fprintf(yyout, ", ");
        this->use_list[2]->output();
    }
    if (this->use_list[1]->isReg() || this->use_list[1]->isVReg())
    {
        fprintf(yyout, "]");
    }
    fprintf(yyout, "\n");
}

MovMInstruction::MovMInstruction(MachineBlock* p, int op, MachineOperand* dst, MachineOperand* src, int cond)
{
    this->parent = p;
    this->type = MachineInstruction::MOV;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src);
    dst->setParent(this);
    src->setParent(this);
}

void MovMInstruction::output() 
{
    fprintf(yyout, "\tmov");
    PrintCond();
    fprintf(yyout, " ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, "\n");
}

BranchMInstruction::BranchMInstruction(MachineBlock* p, int op, MachineOperand* dst, int cond)
{
    this->type = MachineInstruction::BRANCH;
    this->cond = cond;
    this->parent = p;
    this->op = op;
    this->use_list.push_back(dst);
    dst->setParent(this);
}

void BranchMInstruction::output() 
{
    switch (op) 
    {
        case B:
            fprintf(yyout, "\tb");
            PrintCond();
            fprintf(yyout, " ");
            this->use_list[0]->output();
            fprintf(yyout, "\n");
            break;
        case BX:
            fprintf(yyout, "\tbx");
            PrintCond();
            fprintf(yyout, " ");
            this->use_list[0]->output();
            fprintf(yyout, "\n");
            break;
        case BL:
            fprintf(yyout, "\tbl");
            PrintCond();
            fprintf(yyout, " ");
            this->use_list[0]->output();
            fprintf(yyout, "\n");
            break;
    }
}

CmpMInstruction::CmpMInstruction(MachineBlock* p, MachineOperand* src1, MachineOperand* src2, int cond) 
{
    this->parent = p;
    this->type = MachineInstruction::CMP;
    this->op = -1;
    this->cond = cond;
    p->setCmpNo(cond);
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    src1->setParent(this);
    src2->setParent(this);
}

void CmpMInstruction::output() 
{
    fprintf(yyout, "\tcmp ");
    this->use_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[1]->output();
    fprintf(yyout, "\n");
}

StackMInstrcuton::StackMInstrcuton(MachineBlock* p, int op, std::vector<MachineOperand*> srcs, MachineOperand* src, MachineOperand* src1, int cond) 
{
    this->parent = p;
    this->type = MachineInstruction::STACK;
    this->op = op;
    this->cond = cond;
    if (srcs.size() != 0)
    {
        for (long unsigned int i = 0; i != srcs.size(); i++)
        {
            this->use_list.push_back(srcs[i]);
        }
    }
    this->use_list.push_back(src);
    src->setParent(this);
    if (src1 != nullptr) 
    {
        this->use_list.push_back(src1);
        src1->setParent(this);
    }
}

void StackMInstrcuton::output() 
{
    switch (op) 
    {
        case PUSH:
            fprintf(yyout, "\tpush ");
            break;
        case POP:
            fprintf(yyout, "\tpop ");
            break;
    }
    fprintf(yyout, "{");
    this->use_list[0]->output();
    long unsigned int index = 1;
    while (index < use_list.size()) 
    {
        fprintf(yyout, ", ");
        this->use_list[index]->output();
        index++;
    }
    fprintf(yyout, "}\n");
}

MachineFunction::MachineFunction(MachineUnit* p, SymbolEntry* sym_ptr) 
{
    this->parent = p;
    this->sym_ptr = sym_ptr;
    this->stack_size = 0;
    this->paramsNum = ((FunctionType*)(sym_ptr->getType()))->getParamsSe().size();
};

void MachineFunction::output() 
{
    fprintf(yyout, "\t.global %s\n", this->sym_ptr->toStr().c_str() + 1);
    fprintf(yyout, "\t.type %s , %%function\n", this->sym_ptr->toStr().c_str() + 1);
    fprintf(yyout, "%s:\n", this->sym_ptr->toStr().c_str() + 1);
    // TODO
    /* Hint:
     *  1. Save fp
     *  2. fp = sp
     *  3. Save callee saved register
     *  4. Allocate stack space for local variable */

    // Traverse all the block in block_list to print assembly code.
    MachineOperand *fp = new MachineOperand(MachineOperand::REG, 11);
    MachineOperand *sp = new MachineOperand(MachineOperand::REG, 13);
    MachineOperand *lr = new MachineOperand(MachineOperand::REG, 14);
    (new StackMInstrcuton(nullptr, StackMInstrcuton::PUSH, getSavedRegs(), fp, lr)) ->output();
    (new MovMInstruction(nullptr, MovMInstruction::MOV, fp, sp))->output();

    (new BinaryMInstruction(nullptr, BinaryMInstruction::SUB, sp, sp, new MachineOperand(MachineOperand::IMM, AllocSpace(0))))->output();
    
    for (long unsigned int i = 0; i < block_list.size(); i++) 
    {
        block_list[i]->output();
    }
    fprintf(yyout, "\n");
}

std::vector<MachineOperand*> MachineFunction::getSavedRegs() 
{
    std::vector<MachineOperand*> regs;
    for (std::set<int>::iterator it = saved_regs.begin(); it != saved_regs.end(); it++) 
    {
        MachineOperand * reg = new MachineOperand(MachineOperand::REG, *it);
        regs.push_back(reg);
    }
    return regs;
}

void MachineUnit::PrintGlobalDecl() 
{
    std::vector<int> constIdx;
    std::vector<int> zeroIdx;
    if (global_list.empty() == false)           //打印全局变量列表
        fprintf(yyout, ".data\n\n");
    for (long unsigned int i = 0; i < global_list.size(); i++) 
    {
        IdentifierSymbolEntry* se = (IdentifierSymbolEntry*)global_list[i];
        if (se->getConst()) 
        {
            constIdx.push_back(i);
        }  
        else if (se->isAllZero()) 
        {
            zeroIdx.push_back(i);
        } 
        else
        {
            fprintf(yyout, ".global %s\n", se->toStr().c_str());
            fprintf(yyout, ".size %s, %d\n", se->toStr().c_str(), se->getType()->getSize() / 8);
            fprintf(yyout, "%s:\n", se->toStr().c_str());
            fprintf(yyout, "\t.word %d\n", se->getValue());
        }
    }
    if (zeroIdx.empty() == false) 
    {
        for (long unsigned int i = 0; i < zeroIdx.size(); i++) 
        {
            IdentifierSymbolEntry* se = (IdentifierSymbolEntry*)global_list[zeroIdx[i]];
            if (se->getType()->isArray()) 
            {
                fprintf(yyout, "\t.comm %s, %d, 4\n", se->toStr().c_str(), se->getType()->getSize() / 8);
            }
        }
    }
    if (constIdx.empty() == false) 
    {        //打印常量列表
        fprintf(yyout, ".section .rodata\n\n");
        for (long unsigned int i = 0; i < constIdx.size(); i++) 
        {
            IdentifierSymbolEntry* se = (IdentifierSymbolEntry*)global_list[constIdx[i]];
            fprintf(yyout, ".global %s\n", se->toStr().c_str());
            fprintf(yyout, ".size %s, %d\n", se->toStr().c_str(), se->getType()->getSize() / 8);
            fprintf(yyout, "%s:\n", se->toStr().c_str());
            fprintf(yyout, "\t.word %d\n", se->getValue());
        }
    }
}

void MachineUnit::output() 
{
    // TODO
    /* Hint:
     * 1. You need to print global variable/const declarition code;
     * 2. Traverse all the function in func_list to print assembly code;
     * 3. Don't forget print bridge label at the end of assembly code!! */
    fprintf(yyout, "\t.arch armv8-a\n");
    fprintf(yyout, "\t.arch_extension crc\n");
    fprintf(yyout, "\t.arm\n");
    PrintGlobalDecl();
    fprintf(yyout, "\t.text\n");
    for (auto iter : func_list)
        iter->output();
    printGlobal();
}

void MachineUnit::insertGlobal(SymbolEntry* se) 
{
    global_list.push_back(se);
}

void MachineUnit::printGlobal()
{
    for (auto s : global_list) {
        IdentifierSymbolEntry* se = (IdentifierSymbolEntry*)s;
        fprintf(yyout, "addr_%s%d:\n", se->toStr().c_str(), gnumber);
        fprintf(yyout, "\t.word %s\n", se->toStr().c_str());
    }
    gnumber++;
}

bool MachineOperand::operator==(const MachineOperand& a) const 
{
    if (this->type != a.type)
    {
        return false;
    }
    if (this->type == IMM)
    {
        return this->val == a.val;
    }
    return this->reg_no == a.reg_no;
}

bool MachineOperand::operator<(const MachineOperand& a) const 
{
    if (this->type == a.type) {
        if (this->type == IMM)
            return this->val < a.val;
        return this->reg_no < a.reg_no;
    }
    return this->type < a.type;

    if (this->type != a.type)
        return false;
    if (this->type == IMM)
        return this->val == a.val;
    return this->reg_no == a.reg_no;
}