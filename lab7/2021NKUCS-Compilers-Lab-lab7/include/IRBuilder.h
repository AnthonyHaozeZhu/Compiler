#ifndef __IRBUILDER_H__
#define __IRBUILDER_H__

class Unit;
class Function;
class BasicBlock;

class IRBuilder
{
private:
    Unit *unit;
    BasicBlock *insertBB;   // The current basicblock that instructions should be inserted into.
    bool gen_br;
public:
    IRBuilder(Unit*unit) : unit(unit){gen_br = false;};
    void setInsertBB(BasicBlock*bb){insertBB = bb;};
    Unit* getUnit(){return unit;};
    BasicBlock* getInsertBB(){return insertBB;};
    bool genBr() const {return gen_br;};
    void setGenBr(bool gen_br) {this->gen_br = gen_br;};
};

#endif