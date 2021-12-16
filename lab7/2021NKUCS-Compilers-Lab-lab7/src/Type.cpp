#include "Type.h"
#include <sstream>
#include <memory.h>

IntType IntType::commonInt32(32);
IntType IntType::commonInt1(1);
VoidType VoidType::commonVoid;
FunctionType::HashEntry* FunctionType::hashTable[256] = {};
PointerType::HashEntry* PointerType::hashTable[256] = {};
ArrayType::HashEntry* ArrayType::hashTable[256] = {};

bool Type::isI1() const 
{
    return kind == INT && 
    dynamic_cast<const IntType*>(this)->getNumBits() == 1;
}

bool Type::isI32() const 
{
    return kind == INT && 
    dynamic_cast<const IntType*>(this)->getNumBits() == 32;
}

std::string IntType::toStr()
{
    std::ostringstream buffer;
    buffer << "i" << numBits;
    return buffer.str();
}

IntType* IntType::get(int numBits)
{
    switch (numBits)
    {
        case 1: return &commonInt1;
        case 32: return &commonInt32;
        default: return &commonInt32;
    }
}

std::string VoidType::toStr()
{
    return "void";
}

std::string FunctionType::toStr()
{
    std::ostringstream buffer;
    buffer << returnType->toStr();
    buffer << paramsToStr();
    return buffer.str();
}

std::string FunctionType::paramsToStr() 
{
    std::ostringstream buffer;
    if(paramsType.empty())
        buffer << "()";
    else
    {
        buffer << "(" << paramsType[0]->toStr();
        for (size_t i = 1; i < paramsType.size(); i++)
            buffer << ", " << paramsType[i]->toStr();
        buffer << ")";
    }
    return buffer.str();
}

std::string PointerType::toStr()
{
    std::ostringstream buffer;
    buffer << valueType->toStr() << "*";
    return buffer.str();
}

std::string ArrayType::toStr()
{
    std::ostringstream buffer;
    buffer << "[" << numEle << " x " << eleType->toStr() << "]";
    return buffer.str();
}

int FunctionType::hashFunc(Type *retType, std::vector<Type*> &paramType)
{
    unsigned long long ret = (unsigned long long)retType;
    for(auto type:paramType)
        ret += (unsigned long long)type;
    return ret % 256;
}

FunctionType* FunctionType::get(Type*retType, std::vector<Type*> paramsType)
{
    int hash_value = hashFunc(retType, paramsType);
    HashEntry* hash_entry = hashTable[hash_value];
    while(hash_entry)
    {
        FunctionType*funcType = hash_entry->funcType;
        if(retType == funcType->getRetType() && paramsType == funcType->getParamsType())
            return funcType;
        hash_entry = hash_entry->next;
    }
    FunctionType *funcType = new FunctionType(retType, paramsType);
    HashEntry *newEntry = new HashEntry({funcType, hashTable[hash_value]});
    hashTable[hash_value] = newEntry;
    return funcType;
}

PointerType* PointerType::get(Type*valType)
{
    int hash_value = hashFunc(valType);
    auto hash_entry = hashTable[hash_value];
    while(hash_entry)
    {
        PointerType *ptr;
        ptr = hash_entry->ptrType;
        if(valType == ptr->getValueType())
            return ptr;
        hash_entry = hash_entry->next;
    }
    PointerType *ptrType = new PointerType(valType);
    HashEntry *newEntry = new HashEntry({ptrType, hashTable[hash_value]});
    hashTable[hash_value] = newEntry;
    return ptrType;
}

ArrayType* ArrayType::get(Type*eleType, int numEle)
{
    int hash_value = hashFunc(eleType, numEle);
    auto hash_entry = hashTable[hash_value];
    while(hash_entry)
    {
        ArrayType *arrType;
        arrType = hash_entry->arrayType;
        if(eleType == arrType->getEleType() && numEle == arrType->getEleNum())
            return arrType;
        hash_entry = hash_entry->next;
    }
    ArrayType *arrType = new ArrayType(eleType, numEle);
    HashEntry *newEntry = new HashEntry({arrType, hashTable[hash_value]});
    hashTable[hash_value] = newEntry;
    return arrType;
}
