#include "Instruction.h"
#include <iostream>
#include <sstream>
#include "BasicBlock.h"
#include "Function.h"
#include "Type.h"
extern FILE* yyout;

Instruction::Instruction(unsigned instType, BasicBlock* insert_bb) 
{
    prev = next = this;
    opcode = -1;
    this->instType = instType;
    if (insert_bb != nullptr) {
        insert_bb->insertBack(this);
        parent = insert_bb;
    }
}

Instruction::~Instruction() 
{
    parent->remove(this);
}

BasicBlock* Instruction::getParent() 
{
    return parent;
}

void Instruction::setParent(BasicBlock* bb) 
{
    parent = bb;
}

void Instruction::setNext(Instruction* inst) 
{
    next = inst;
}

void Instruction::setPrev(Instruction* inst) 
{
    prev = inst;
}

Instruction* Instruction::getNext() 
{
    return next;
}

Instruction* Instruction::getPrev() 
{
    return prev;
}

BinaryInstruction::BinaryInstruction(unsigned opcode, Operand* dst, Operand* src1, Operand* src2, BasicBlock* insert_bb) : Instruction(BINARY, insert_bb) 
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

BinaryInstruction::~BinaryInstruction() 
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void BinaryInstruction::output() const {}

CmpInstruction::CmpInstruction(unsigned opcode, Operand* dst, Operand* src1, Operand* src2, BasicBlock* insert_bb) : Instruction(CMP, insert_bb) {
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

CmpInstruction::~CmpInstruction() {}

void CmpInstruction::output() const {}

UncondBrInstruction::UncondBrInstruction(BasicBlock* to, BasicBlock* insert_bb) : Instruction(UNCOND, insert_bb) 
{
    branch = to;
}

void UncondBrInstruction::output() const {}

void UncondBrInstruction::setBranch(BasicBlock* bb) 
{
    branch = bb;
}

BasicBlock* UncondBrInstruction::getBranch() 
{
    return branch;
}

CondBrInstruction::CondBrInstruction(BasicBlock* true_branch, BasicBlock* false_branch, Operand* cond, BasicBlock* insert_bb) : Instruction(COND, insert_bb)
 {
    this->true_branch = true_branch;
    this->false_branch = false_branch;
    cond->addUse(this);
    operands.push_back(cond);
}

CondBrInstruction::~CondBrInstruction() {}

void CondBrInstruction::output() const {}

void CondBrInstruction::setFalseBranch(BasicBlock* bb) 
{
    false_branch = bb;
}

BasicBlock* CondBrInstruction::getFalseBranch() 
{
    return false_branch;
}

void CondBrInstruction::setTrueBranch(BasicBlock* bb) 
{
    true_branch = bb;
}

BasicBlock* CondBrInstruction::getTrueBranch() 
{
    return true_branch;
}

RetInstruction::RetInstruction(Operand* src, BasicBlock* insert_bb) : Instruction(RET, insert_bb) {
    if (src != nullptr) 
    {
        operands.push_back(src);
        src->addUse(this);
    }
}

RetInstruction::~RetInstruction() {}

void RetInstruction::output() const {}

AllocaInstruction::AllocaInstruction(Operand* dst, SymbolEntry* se, BasicBlock* insert_bb) : Instruction(ALLOCA, insert_bb) 
{
    operands.push_back(dst);
    dst->setDef(this);
    this->se = se;
}

AllocaInstruction::~AllocaInstruction() {}

void AllocaInstruction::output() const {}

LoadInstruction::LoadInstruction(Operand* dst, Operand* src_addr, BasicBlock* insert_bb) : Instruction(LOAD, insert_bb) 
{
    operands.push_back(dst);
    operands.push_back(src_addr);
    dst->setDef(this);
    src_addr->addUse(this);
}

LoadInstruction::~LoadInstruction() {}

void LoadInstruction::output() const {}

StoreInstruction::StoreInstruction(Operand* dst_addr, Operand* src, BasicBlock* insert_bb) : Instruction(STORE, insert_bb) 
{
    operands.push_back(dst_addr);
    operands.push_back(src);
    dst_addr->addUse(this);
    src->addUse(this);
}

StoreInstruction::~StoreInstruction() {}

void StoreInstruction::output() const {}

MachineOperand* Instruction::genMachineOperand(Operand* ope) 
{
    auto se = ope->getEntry();
    MachineOperand* mope = nullptr;
    if (se->isConstant())
        mope = new MachineOperand(MachineOperand::IMM, dynamic_cast<ConstantSymbolEntry*>(se)->getValue());
    else if (se->isTemporary())
        mope = new MachineOperand(MachineOperand::VREG, dynamic_cast<TemporarySymbolEntry*>(se)->getLabel());
    else if (se->isVariable()) 
    {
        auto id_se = dynamic_cast<IdentifierSymbolEntry*>(se);
        if (id_se->isGlobal())
            mope = new MachineOperand(id_se->toStr().c_str());
        else if (id_se->isParam()) 
        {
            // TODO
            if (id_se->getParamNo() < 4)
                mope = new MachineOperand(MachineOperand::REG, id_se->getParamNo());
            else
                mope = new MachineOperand(MachineOperand::REG, 3);
        } 
        else
            exit(0);
    }
    return mope;
}

MachineOperand* Instruction::genMachineReg(int reg) 
{
    return new MachineOperand(MachineOperand::REG, reg);
}

MachineOperand* Instruction::genMachineVReg() 
{
    return new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());
}

MachineOperand* Instruction::genMachineImm(int val) 
{
    return new MachineOperand(MachineOperand::IMM, val);
}

MachineOperand* Instruction::genMachineLabel(int block_no) 
{
    std::ostringstream buf;
    buf << ".L" << block_no;
    std::string label = buf.str();
    return new MachineOperand(label);
}

void AllocaInstruction::genMachineCode(AsmBuilder* builder) 
{
    /* HINT:
     * Allocate stack space for local variabel
     * Store frame offset in symbol entry */
    auto cur_func = builder->getFunction();
    int size = se->getType()->getSize() / 8;
    if (size < 0)
        size = 4;
    int offset = cur_func->AllocSpace(size);
    dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())
        ->setOffset(-offset);
}

void LoadInstruction::genMachineCode(AsmBuilder* builder) 
{
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    if (operands[1]->getEntry()->isVariable() && dynamic_cast<IdentifierSymbolEntry*>(operands[1]->getEntry())->isGlobal()) 
    {
        auto dst = genMachineOperand(operands[0]);
        auto internal_reg1 = genMachineVReg();
        auto internal_reg2 = new MachineOperand(*internal_reg1);
        auto src = genMachineOperand(operands[1]);
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);
        cur_block->InsertInst(cur_inst);
        cur_inst = new LoadMInstruction(cur_block, dst, internal_reg2);
        cur_block->InsertInst(cur_inst);
    }
    // Load local operand
    else if (operands[1]->getEntry()->isTemporary() && operands[1]->getDef() && operands[1]->getDef()->isAlloc()) 
    {
        auto dst = genMachineOperand(operands[0]);
        auto src1 = genMachineReg(11);
        int off = dynamic_cast<TemporarySymbolEntry*>(operands[1]->getEntry())
                      ->getOffset();
        auto src2 = genMachineImm(off);
        if (off > 255 || off < -255) {
            auto operand = genMachineVReg();
            cur_block->InsertInst(
                (new LoadMInstruction(cur_block, operand, src2)));
            src2 = operand;
        }
        cur_inst = new LoadMInstruction(cur_block, dst, src1, src2);
        cur_block->InsertInst(cur_inst);
    }
    // Load operand from temporary variable
    else 
    {
        // example: load r1, [r0]
        auto dst = genMachineOperand(operands[0]);
        auto src = genMachineOperand(operands[1]);
        cur_inst = new LoadMInstruction(cur_block, dst, src);
        cur_block->InsertInst(cur_inst);
    }
}

void StoreInstruction::genMachineCode(AsmBuilder* builder)
 {
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    auto dst = genMachineOperand(operands[0]);
    auto src = genMachineOperand(operands[1]);
    if (operands[1]->getEntry()->isConstant()) 
    {
        auto dst1 = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, dst1, src);
        cur_block->InsertInst(cur_inst);
        src = new MachineOperand(*dst1);
    }
    if (operands[0]->getEntry()->isTemporary() && operands[0]->getDef() && operands[0]->getDef()->isAlloc()) 
    {
        auto src1 = genMachineReg(11);
        int off = dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->getOffset();
        auto src2 = genMachineImm(off);
        cur_inst = new StoreMInstruction(cur_block, src, src1, src2);
        cur_block->InsertInst(cur_inst);
    }
    else if (operands[0]->getEntry()->isVariable() && dynamic_cast<IdentifierSymbolEntry*>(operands[0]->getEntry())->isGlobal()) 
    {
        auto temp_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, temp_reg, dst);
        cur_block->InsertInst(cur_inst);
        cur_inst = new StoreMInstruction(cur_block, src, temp_reg);
        cur_block->InsertInst(cur_inst);
    }
    else if (operands[0]->getType()->isPtr()) 
    {
        cur_inst = new StoreMInstruction(cur_block, src, dst);
        cur_block->InsertInst(cur_inst);
    }
}

void BinaryInstruction::genMachineCode(AsmBuilder* builder) 
{
    // complete other instructions
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src1 = genMachineOperand(operands[1]);
    auto src2 = genMachineOperand(operands[2]);
    /* HINT:
     * The source operands of ADD instruction in ir code both can be immediate
     * num. However, it's not allowed in assembly code. So you need to insert
     * LOAD/MOV instrucrion to load immediate num into register. As to other
     * instructions, such as MUL, CMP, you need to deal with this situation,
     * too.*/
    MachineInstruction* cur_inst = nullptr;
    if (src1->isImm()) 
    {
        auto temp_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, temp_reg, src1);
        cur_block->InsertInst(cur_inst);
        src1 = new MachineOperand(*temp_reg);
    }
    if (src2->isImm()) 
    {
            auto temp_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, temp_reg, src2);
            cur_block->InsertInst(cur_inst);
            src2 = new MachineOperand(*temp_reg);
    }
    switch (opcode) 
    {
        case ADD:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, src1, src2);
            break;
        case SUB:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, src2);
            break;
        case AND:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::AND, dst, src1, src2);
            break;
        case OR:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::OR, dst, src1, src2);
            break;
        case MUL:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, dst, src1, src2);
            break;
        case DIV:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, dst, src1, src2);
            break;
        case MOD: {
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, dst, src1, src2);
            MachineOperand* dst1 = new MachineOperand(*dst);
            src1 = new MachineOperand(*src1);
            src2 = new MachineOperand(*src2);
            cur_block->InsertInst(cur_inst);
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, dst1, dst, src2);
            cur_block->InsertInst(cur_inst);
            dst = new MachineOperand(*dst1);
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, dst1);
            break;
        }
        default:
            break;
    }
    cur_block->InsertInst(cur_inst);
}

void CmpInstruction::genMachineCode(AsmBuilder* builder) 
{
    auto cur_block = builder->getBlock();
    auto src1 = genMachineOperand(operands[1]);
    auto src2 = genMachineOperand(operands[2]);
    MachineInstruction* cur_inst = nullptr;
    if (src1->isImm()) 
    {
        auto temp_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, temp_reg, src1);
        cur_block->InsertInst(cur_inst);
        src1 = new MachineOperand(*temp_reg);
    }
    if (src2->isImm()) 
    {
        auto temp_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, temp_reg, src2);
        cur_block->InsertInst(cur_inst);
        src2 = new MachineOperand(*temp_reg);
    }
    cur_inst = new CmpMInstruction(cur_block, src1, src2, opcode);
    cur_block->InsertInst(cur_inst);
    if (opcode >= CmpInstruction::L && opcode <= CmpInstruction::GE) 
    {
        auto dst = genMachineOperand(operands[0]);
        auto trueOperand = genMachineImm(1);
        auto falseOperand = genMachineImm(0);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst,trueOperand, opcode);
        cur_block->InsertInst(cur_inst);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseOperand, 7 - opcode);
        cur_block->InsertInst(cur_inst);
    }
}

void UncondBrInstruction::genMachineCode(AsmBuilder* builder) 
{
    auto cur_block = builder->getBlock();
    std::string s="";
    s =s+ ".L" + std::to_string(branch->getNo());
    MachineOperand* dst = new MachineOperand(s);
    auto cur_inst =new BranchMInstruction(cur_block, BranchMInstruction::B, dst);
    cur_block->InsertInst(cur_inst);
}

void CondBrInstruction::genMachineCode(AsmBuilder* builder) 
{
    auto cur_block = builder->getBlock();
    std::string s="";
    s =s+ ".L" + std::to_string(true_branch->getNo());
    MachineOperand* dst = new MachineOperand(s);
    auto cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B,dst, cur_block->getCmpNo());
    cur_block->InsertInst(cur_inst);
    s="";
    s =s+ ".L" + std::to_string(false_branch->getNo());
    dst = new MachineOperand(s);
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, dst);
    cur_block->InsertInst(cur_inst);
}

void RetInstruction::genMachineCode(AsmBuilder* builder) 
{
    // TODO
    /* HINT:
     * 1. Generate mov instruction to save return value in r0
     * 2. Restore callee saved registers and sp, fp
     * 3. Generate bx instruction */
    auto cur_block = builder->getBlock();
    if (!operands.empty()) 
    {
        auto dst = new MachineOperand(MachineOperand::REG, 0);
        auto src = genMachineOperand(operands[0]);
        auto cur_inst =new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src);
        cur_block->InsertInst(cur_inst);
    }
    auto cur_func = builder->getFunction();
    auto sp = new MachineOperand(MachineOperand::REG, 13);
    auto size =new MachineOperand(MachineOperand::IMM, cur_func->AllocSpace(0));
    auto cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD,sp, sp, size);
    cur_block->InsertInst(cur_inst);
    auto lr = new MachineOperand(MachineOperand::REG, 14);
    auto cur_inst2 =new BranchMInstruction(cur_block, BranchMInstruction::BX, lr);
    cur_block->InsertInst(cur_inst2);
}

CallInstruction::CallInstruction(Operand* dst,SymbolEntry* func,std::vector<Operand*> params,BasicBlock* insert_bb): Instruction(CALL, insert_bb), func(func), dst(dst) 
{
    operands.push_back(dst);
    if (dst)
        dst->setDef(this);
    for (auto param : params) {
        operands.push_back(param);
        param->addUse(this);
    }
}

void CallInstruction::output() const 
{
    fprintf(yyout, "  ");
    if (operands[0])
        fprintf(yyout, "%s = ", operands[0]->toStr().c_str());
    FunctionType* type = (FunctionType*)(func->getType());
    fprintf(yyout, "call %s %s(", type->getRetType()->toStr().c_str(),func->toStr().c_str());
    for (long unsigned int i = 1; i < operands.size(); i++) {
        if (i != 1)
            fprintf(yyout, ", ");
        fprintf(yyout, "%s %s", operands[i]->getType()->toStr().c_str(),operands[i]->toStr().c_str());
    }
    fprintf(yyout, ")\n");
}

CallInstruction::~CallInstruction() {}

ZextInstruction::ZextInstruction(Operand* dst, Operand* src, BasicBlock* insert_bb) : Instruction(ZEXT, insert_bb) {
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void ZextInstruction::output() const {}

ZextInstruction::~ZextInstruction() {}

XorInstruction::XorInstruction(Operand* dst,
                               Operand* src,
                               BasicBlock* insert_bb) : Instruction(XOR, insert_bb) {
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void XorInstruction::output() const {}

XorInstruction::~XorInstruction() {}

GepInstruction::GepInstruction(Operand* dst, Operand* arr, Operand* idx, BasicBlock* insert_bb, bool paramFirst) : Instruction(GEP, insert_bb), paramFirst(paramFirst)
{
    operands.push_back(dst);
    operands.push_back(arr);
    operands.push_back(idx);
    dst->setDef(this);
    arr->addUse(this);
    idx->addUse(this);
    first = false;
    init = nullptr;
    last = false;
}

void GepInstruction::output() const {}

GepInstruction::~GepInstruction() {}

void CallInstruction::genMachineCode(AsmBuilder* builder) 
{
    auto cur_block = builder->getBlock();
    MachineOperand* operand;  
    MachineInstruction* cur_inst;
    int idx = 0;
    auto it = operands.begin();
    it++;
    for (; it != operands.end(); it++, idx++) 
    {
        if (idx == 4)
            break;
        operand = genMachineReg(idx);
        auto src = genMachineOperand(operands[idx+1]);
        if (src->isImm() ) {
            cur_inst = new LoadMInstruction(cur_block, operand, src);
        } else
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV,
                                           operand, src);
        cur_block->InsertInst(cur_inst);
    }
    for (int i = operands.size() - 1; i > 4; i--) 
    {
        operand = genMachineOperand(operands[i]);
        if (operand->isImm()) {
            auto dst = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, dst, operand);
            cur_block->InsertInst(cur_inst);
            operand = dst;
        }
        std::vector<MachineOperand*> vec;
        cur_inst = new StackMInstrcuton(cur_block, StackMInstrcuton::PUSH, vec, operand);
        cur_block->InsertInst(cur_inst);
    }
    auto label = new MachineOperand(func->toStr().c_str());
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::BL, label);
    cur_block->InsertInst(cur_inst);
    if (operands.size() > 5) 
    {
        auto off = genMachineImm((operands.size() - 5) * 4);
        auto sp = new MachineOperand(MachineOperand::REG, 13);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD,sp, sp, off);
        cur_block->InsertInst(cur_inst);
    }
    if (dst) 
    {
        operand = genMachineOperand(dst);
        auto r0 = new MachineOperand(MachineOperand::REG, 0);
        cur_inst =
            new MovMInstruction(cur_block, MovMInstruction::MOV, operand, r0);
        cur_block->InsertInst(cur_inst);
    }
}

void ZextInstruction::genMachineCode(AsmBuilder* builder) 
{
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src = genMachineOperand(operands[1]);
    auto cur_inst =new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src);
    cur_block->InsertInst(cur_inst);
}

void XorInstruction::genMachineCode(AsmBuilder* builder) 
{
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto trueOperand = genMachineImm(1);
    auto falseOperand = genMachineImm(0);
    auto cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst,trueOperand, MachineInstruction::EQ);
    cur_block->InsertInst(cur_inst);
    cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst,falseOperand, MachineInstruction::NE);
    cur_block->InsertInst(cur_inst);
}

void GepInstruction::genMachineCode(AsmBuilder* builder) 
{
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst;
    auto dst = genMachineOperand(operands[0]);
    auto idx = genMachineOperand(operands[2]);
    MachineOperand* base = nullptr;
    int size;
    auto idx1 = genMachineVReg();
    if (idx->isImm()) 
    {
        cur_inst = new LoadMInstruction(cur_block, idx1, idx);
        idx = new MachineOperand(*idx1);
        cur_block->InsertInst(cur_inst);
    }
    if (paramFirst) 
    {
        size = ((PointerType*)(operands[1]->getType()))->getType()->getSize() / 8;
    } 
    else 
    {
        if (first) 
        {
            base = genMachineVReg();
            if (operands[1]->getEntry()->isVariable() && ((IdentifierSymbolEntry*)(operands[1]->getEntry())) ->isGlobal()) 
            {
                auto src = genMachineOperand(operands[1]);
                cur_inst = new LoadMInstruction(cur_block, base, src);
            } 
            else 
            {
                int offset = ((TemporarySymbolEntry*)(operands[1]->getEntry())) ->getOffset();
                cur_inst = new LoadMInstruction(cur_block, base, genMachineImm(offset));
            }
            cur_block->InsertInst(cur_inst);
        }
        ArrayType* type = (ArrayType*)(((PointerType*)(operands[1]->getType()))->getType());
        size = type->getElementType()->getSize() / 8;
    }
    auto size1 = genMachineVReg();
    cur_inst = new LoadMInstruction(cur_block, size1, genMachineImm(size));
    cur_block->InsertInst(cur_inst);
    auto off = genMachineVReg();
    cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, off, idx, size1);
    off = new MachineOperand(*off);
    cur_block->InsertInst(cur_inst);
    if (paramFirst || !first) 
    {
        auto arr = genMachineOperand(operands[1]);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, arr, off);
        cur_block->InsertInst(cur_inst);
    } 
    else 
    {
        auto addr = genMachineVReg();
        auto base1 = new MachineOperand(*base);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, addr, base1, off);
        cur_block->InsertInst(cur_inst);
        addr = new MachineOperand(*addr);
        if (operands[1]->getEntry()->isVariable() && ((IdentifierSymbolEntry*)(operands[1]->getEntry()))->isGlobal()) 
        {
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, addr);
        } 
        else 
        {
            auto fp = genMachineReg(11);
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, fp, addr);
        }
        cur_block->InsertInst(cur_inst);
    }
}
