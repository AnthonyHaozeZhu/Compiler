#include "LinearScan.h"
#include <algorithm>
#include <iostream>
#include "LiveVariableAnalysis.h"
#include "MachineCode.h"

LinearScan::LinearScan(MachineUnit* unit) {   //初始化寄存器
    this->unit = unit;
    for (int i = 4; i < 11; i++)
        regs.push_back(i);
}

void LinearScan::allocateRegisters() {
    for (auto& f : unit->getFuncs()) {
        func = f;
        bool success;
        success = false;
        while (!success)  // repeat until all vregs can be mapped
        {
            computeLiveIntervals();
            success = linearScanRegisterAllocation();
            if (success)  // all vregs can be mapped to real regs
                modifyCode();
            else  // spill vregs that can't be mapped to real regs
                genSpillCode();
        }
    }
}

void LinearScan::makeDuChains() {   //构建定义-引用链
    LiveVariableAnalysis lva; 
    lva.pass(func);             //针对单个函数块的活跃性分析
    du_chains.clear();
    int i = 0;
    std::map<MachineOperand, std::set<MachineOperand*>> liveVar;
    for (auto& bb : func->getBlocks()) {
        liveVar.clear();
        for (auto& t : bb->getLiveOut())    //将每个块将out的全加入活跃变量map中
            liveVar[*t].insert(t);
        int no;
        no = i = bb->getInsts().size() + i;     //记录指令个数
        for (auto inst = bb->getInsts().rbegin(); inst != bb->getInsts().rend();
             inst++) {                //对block中的每条指令
            (*inst)->setNo(no--);
            for (auto& def : (*inst)->getDef()) {    //对于每条指令中的def（即该指令新定义的目的操作数）
                if (def->isVReg()) {                 //如果在虚拟寄存器中
                    auto& uses = liveVar[*def];     //如果定义的操作数也在块out队列中，加入du链
                    du_chains[def].insert(uses.begin(), uses.end());       //整个函数创建livevar就是为duchains服务的
                    auto& kill = lva.getAllUses()[*def];             //该block杀死的def，即该def没有出现在out
                    std::set<MachineOperand*> res;
                    set_difference(uses.begin(), uses.end(), kill.begin(),    //uses<-livevar[def]<-liveout
                                   kill.end(), inserter(res, res.end()));     //即用liveout减杀死的
                    liveVar[*def] = res;        //将差集放入def的livevar map队列中
                }
            }
            for (auto& use : (*inst)->getUse()) {
                if (use->isVReg())
                    liveVar[*use].insert(use);    //该块对源操作数use进行了计算，后边如果没有定义的话，记为该block产生了该use
            }
        }
    }
}

void LinearScan::computeLiveIntervals() {
    makeDuChains();
    intervals.clear();         //intervals用于记录当前未分配寄存器的活跃区间
    for (auto& du_chain : du_chains) {
        int t = -1;
        for (auto& use : du_chain.second)
            t = std::max(t, use->getParent()->getNo());
        Interval* interval = new Interval({du_chain.first->getParent()->getNo(),
            t, false, 0, 0, {du_chain.first}, du_chain.second});
        intervals.push_back(interval);
    }
    bool change;
    change = true;
    while (change) {
        change = false;
        std::vector<Interval*> t(intervals.begin(), intervals.end());
        for (size_t i = 0; i < t.size(); i++)
            for (size_t j = i + 1; j < t.size(); j++) {
                Interval* w1 = t[i];
                Interval* w2 = t[j];
                if (**w1->defs.begin() == **w2->defs.begin()) {
                    std::set<MachineOperand*> temp;
                    set_intersection(w1->uses.begin(), w1->uses.end(),
                                     w2->uses.begin(), w2->uses.end(),
                                     inserter(temp, temp.end()));
                    if (!temp.empty()) {
                        change = true;
                        w1->defs.insert(w2->defs.begin(), w2->defs.end());
                        w1->uses.insert(w2->uses.begin(), w2->uses.end());
                        w1->start = std::min(w1->start, w2->start);
                        w1->end = std::max(w1->end, w2->end);
                        auto it =
                            std::find(intervals.begin(), intervals.end(), w2);
                        if (it != intervals.end())
                            intervals.erase(it);
                    }
                }
            }
    }
    sort(intervals.begin(), intervals.end(), compareStart);
}

bool LinearScan::linearScanRegisterAllocation() {
    bool success = true;
    active.clear();
    regs.clear();
    for (int i = 4; i < 11; i++)
        regs.push_back(i);
    for (auto& i : intervals) {
        expireOldIntervals(i);
        if (regs.empty()) {
            spillAtInterval(i);
            success = false;
        } else {
            i->rreg = regs.front();
            regs.erase(regs.begin());
            active.push_back(i);
            sort(active.begin(), active.end(), compareEnd);
        }
    }
    return success;
}

void LinearScan::modifyCode() {
    for (auto& interval : intervals) {
        func->addSavedRegs(interval->rreg);
        for (auto def : interval->defs)
            def->setReg(interval->rreg);
        for (auto use : interval->uses)
            use->setReg(interval->rreg);
    }
}

void LinearScan::genSpillCode() {
    for (auto& interval : intervals) {
        if (!interval->spill)
            continue;
        // TODO
        /* HINT:
         * The vreg should be spilled to memory.
         * 1. insert ldr inst before the use of vreg
         * 2. insert str inst after the def of vreg
         */
        interval->disp = -func->AllocSpace(4);
        auto off = new MachineOperand(MachineOperand::IMM, interval->disp);
        auto fp = new MachineOperand(MachineOperand::REG, 11);
        for (auto use : interval->uses) {
            auto temp = new MachineOperand(*use);
            auto inst = new LoadMInstruction(use->getParent()->getParent(),temp, fp, off);
            use->getParent()->insertBefore(inst);
            
        }
        for (auto def : interval->defs) {
            auto temp = new MachineOperand(*def);
            auto inst = new StoreMInstruction(def->getParent()->getParent(), temp, fp, off);
            def->getParent()->insertAfter(inst);
        }
    }
}

void LinearScan::expireOldIntervals(Interval* interval) {
    auto it = active.begin();
    while (it != active.end()) {
        if((*it)->end < interval->start)
        {
        regs.push_back((*it)->rreg);
        it = active.erase(find(active.begin(), active.end(), *it));
        sort(regs.begin(), regs.end());
        }
        else
            return;

    }
}

void LinearScan::spillAtInterval(Interval* interval) {
    auto spill = active.back();
    if (spill->end > interval->end) {
        spill->spill = true;
        interval->rreg = spill->rreg;
        active.push_back(interval);
        sort(active.begin(), active.end(), compareEnd);
    } else {
        interval->spill = true;
    }
}

bool LinearScan::compareStart(Interval* a, Interval* b) {
    return a->start < b->start;
}

bool LinearScan::compareEnd(Interval* a, Interval* b) {
    return a->end < b->end;
}