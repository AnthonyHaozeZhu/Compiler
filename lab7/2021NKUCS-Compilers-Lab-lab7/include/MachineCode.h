#ifndef __MACHINECODE_H__
#define __MACHINECODE_H__
#include <algorithm>
#include <fstream>
#include <set>
#include <string>
#include <vector>
#include "SymbolTable.h"

/* Hint:
 * MachineUnit: Compiler unit
 * MachineFunction: Function in assembly code
 * MachineInstruction: Single assembly instruction
 * MachineOperand: Operand in assembly instruction, such as immediate number,
 * register, address label */

/* Todo:
 * We only give the example code of "class BinaryMInstruction" and "class
 * AccessMInstruction" (because we believe in you !!!), You need to complete
 * other the member function, especially "output()" , After that, you can use
 * "output()" to print assembly code . */

class MachineUnit;
class MachineFunction;
class MachineBlock;
class MachineInstruction;

class MachineOperand 
{
private:
    MachineInstruction* parent;
    int type;
    int val;            // value of immediate number   
    int reg_no;         // register no
    std::string label;  // address label
public:
    enum { IMM, VREG, REG, LABEL };
    MachineOperand(int tp, int val);
    MachineOperand(std::string label);
    bool operator==(const MachineOperand&) const;
    bool operator<(const MachineOperand&) const;
    bool isImm() { return this->type == IMM; };
    bool isReg() { return this->type == REG; };
    bool isVReg() { return this->type == VREG; };
    bool isLabel() { return this->type == LABEL; };
    int getVal() { return this->val; };
    void setVal(int val) { this->val = val; };
    int getReg() { return this->reg_no; };
    void setReg(int regno) {
        this->type = REG;
        this->reg_no = regno;
    };
    std::string getLabel() { return this->label; };
    void setParent(MachineInstruction* p) { this->parent = p; };
    MachineInstruction* getParent() { return this->parent; };
    void PrintReg();
    void output();
};

class MachineInstruction 
{
protected:
    MachineBlock* parent;
    int no;
    int type;                             // Instruction type
    int cond = MachineInstruction::NONE;  // Instruction execution condition,
                                          // optional !!
    int op;                               // Instruction opcode
    // Instruction operand list, sorted by appearance order in assembly
    // instruction
    std::vector<MachineOperand*> def_list;   
    std::vector<MachineOperand*> use_list;   
    void addDef(MachineOperand* ope) { def_list.push_back(ope); };
    void addUse(MachineOperand* ope) { use_list.push_back(ope); };
    // Print execution code after printing opcode
    void PrintCond();
    enum instType { BINARY, LOAD, STORE, MOV, BRANCH, CMP, STACK };

public:
    enum condType { EQ, NE, LT, LE, GT, GE, NONE };
    virtual void output() = 0;
    void setNo(int no) { this->no = no; };
    int getNo() { return no; };
    std::vector<MachineOperand*>& getDef() { return def_list; };
    std::vector<MachineOperand*>& getUse() { return use_list; };
    void insertBefore(MachineInstruction*);
    void insertAfter(MachineInstruction*);
    MachineBlock* getParent() const { return parent; };
    bool isBX() const { return type == BRANCH && op == 2; };
    bool isStore() const { return type == STORE; };
    bool isAdd() const { return type == BINARY && op == 0; };
};


class BinaryMInstruction : public MachineInstruction 
{
public:
    enum opType { ADD, SUB, MUL, DIV, AND, OR };
    BinaryMInstruction(MachineBlock* p, int op, MachineOperand* dst, MachineOperand* src1, MachineOperand* src2,int cond = MachineInstruction::NONE);
    void output();
};

class LoadMInstruction : public MachineInstruction 
{
public:
    LoadMInstruction(MachineBlock* p, MachineOperand* dst, MachineOperand* src1, MachineOperand* src2 = nullptr, int cond = MachineInstruction::NONE);
    void output();
};

class StoreMInstruction : public MachineInstruction 
{
public:
    StoreMInstruction(MachineBlock* p, MachineOperand* src1, MachineOperand* src2, MachineOperand* src3 = nullptr, int cond = MachineInstruction::NONE);
    void output();
};

class MovMInstruction : public MachineInstruction 
{
public:
    enum opType { MOV, MVN };
    MovMInstruction(MachineBlock* p, int op, MachineOperand* dst, MachineOperand* src, int cond = MachineInstruction::NONE);
    void output();
};

class BranchMInstruction : public MachineInstruction
{
public:
    enum opType { B, BL, BX };
    BranchMInstruction(MachineBlock* p, int op, MachineOperand* dst, int cond = MachineInstruction::NONE);
    void output();
};

class CmpMInstruction : public MachineInstruction 
{
public:
    enum opType { CMP };
    CmpMInstruction(MachineBlock* p, MachineOperand* src1, MachineOperand* src2, int cond = MachineInstruction::NONE);
    void output();
};

class StackMInstrcuton : public MachineInstruction
{
public:
    enum opType { PUSH, POP };
    StackMInstrcuton(MachineBlock* p, int op, std::vector<MachineOperand*> srcs, MachineOperand* src, MachineOperand* src1 = nullptr, int cond = MachineInstruction::NONE);
    void output();
};

class MachineBlock 
{
private:
    MachineFunction* parent;
    int no;
    std::vector<MachineBlock*> pred, succ;        
    std::vector<MachineInstruction*> inst_list;  //指令列表
    std::set<MachineOperand*> live_in;    //活跃
    std::set<MachineOperand*> live_out;  //不活跃
    int cmpno;                   


public:
    std::vector<MachineInstruction*>& getInsts() { return inst_list; };
    std::vector<MachineInstruction*>::iterator begin() {
        return inst_list.begin();
    };
    std::vector<MachineInstruction*>::iterator end() {
        return inst_list.end();
    };
    MachineBlock(MachineFunction* p, int no) {
        this->parent = p;
        this->no = no;
    };
    void InsertInst(MachineInstruction* inst) {
        this->inst_list.push_back(inst);
    };
    void addPred(MachineBlock* p) { this->pred.push_back(p); };
    void addSucc(MachineBlock* s) { this->succ.push_back(s); };
    std::set<MachineOperand*>& getLiveIn() { return live_in; };
    std::set<MachineOperand*>& getLiveOut() { return live_out; };
    std::vector<MachineBlock*>& getPreds() { return pred; };
    std::vector<MachineBlock*>& getSuccs() { return succ; };
    void output();
    int getCmpNo() const { return cmpno; };
    void setCmpNo(int cond) { cmpno = cond; };
    int getSize() const { return inst_list.size(); };
    MachineFunction* getParent() const { return parent; };
};


class MachineFunction 
{
private:
    MachineUnit* parent;
    std::vector<MachineBlock*> block_list; 
    int stack_size;                       //栈大小
    std::set<int> saved_regs;          //寄存器信息
    SymbolEntry* sym_ptr;
    int paramsNum;

public:
    std::vector<MachineBlock*>& getBlocks() { return block_list; };
    std::vector<MachineBlock*>::iterator begin() { return block_list.begin(); };
    std::vector<MachineBlock*>::iterator end() { return block_list.end(); };
    MachineFunction(MachineUnit* p, SymbolEntry* sym_ptr);
    /* HINT:
     * Alloc stack space for local variable;
     * return current frame offset ;
     * we store offset in symbol entry of this variable in function
     * AllocInstruction::genMachineCode() you can use this function in
     * LinearScan::genSpillCode() */
    int AllocSpace(int size) 
    {
        this->stack_size += size;
        return this->stack_size;
    };
    void InsertBlock(MachineBlock* block) 
    {
        this->block_list.push_back(block);
    };
    void addSavedRegs(int regno) { saved_regs.insert(regno); };
    void output();
    std::vector<MachineOperand*> getSavedRegs();
    int getParamsNum() const { return paramsNum; };
    MachineUnit* getParent() const { return parent; };
};


class MachineUnit 
{
private:
    std::vector<SymbolEntry*> global_list;
    std::vector<MachineFunction*> func_list;
    void PrintGlobalDecl();
    int gnumber;   //全局变量个数

public:
    std::vector<MachineFunction*>& getFuncs() { return func_list; };
    std::vector<MachineFunction*>::iterator begin() {
        return func_list.begin();
    };
    std::vector<MachineFunction*>::iterator end() { return func_list.end(); };
    void InsertFunc(MachineFunction* func) { func_list.push_back(func); };
    void output();
    void insertGlobal(SymbolEntry*);
    void printGlobal();
    int getGnumber() const { return gnumber; };
};

#endif