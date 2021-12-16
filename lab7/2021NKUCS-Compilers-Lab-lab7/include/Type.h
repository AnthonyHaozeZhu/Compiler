#ifndef __TYPE_H__
#define __TYPE_H__
#include <vector>
#include <string>

class IntType;
class Type
{
private:
    int kind;
public:
    enum {INT, VOID, FUNC, PTR, ARRAY};
    Type(int kind) : kind(kind) {};
    virtual ~Type() {};
    virtual std::string toStr() = 0;
    bool isInt() const {return kind == INT;};
    bool isI32() const;
    bool isI1() const;
    bool isVoid() const {return kind == VOID;};
    bool isFunc() const {return kind == FUNC;};
};

class IntType : public Type
{
private:
    int numBits;
    static IntType commonInt32;
    static IntType commonInt1;
public:
    IntType(int numBits) : Type(Type::INT), numBits(numBits){};
    int getNumBits() const {return numBits;};
    static IntType* get(int numBits);
    std::string toStr();
};

class VoidType : public Type
{
private:
    static VoidType commonVoid;
public:
    VoidType() : Type(Type::VOID){};
    std::string toStr();
    static VoidType* get() {return &commonVoid;}
};

class FunctionType : public Type
{
private:
    Type *returnType;
    std::vector<Type*> paramsType;
    struct HashEntry
    {
        FunctionType *funcType;
        HashEntry *next;
    };
    static HashEntry* hashTable[256];
    static int hashFunc(Type *retType, std::vector<Type*> &paramsType);
public:
    FunctionType(Type* returnType, std::vector<Type*> paramsType) : 
    Type(Type::FUNC), returnType(returnType), paramsType(paramsType){};
    Type* getRetType() {return returnType;};
    std::vector<Type*>& getParamsType() {return paramsType;};
    std::string toStr();
    std::string paramsToStr();
    static FunctionType* get(Type*retType, std::vector<Type*> paramsType);
};

class PointerType : public Type
{
private:
    Type *valueType;
    struct HashEntry
    {
        PointerType *ptrType;
        HashEntry *next;
    };
    static HashEntry* hashTable[256];
    static int hashFunc(Type *retType) {return (unsigned long long)retType % 256;};
public:
    PointerType(Type* valueType) : Type(Type::PTR) {this->valueType = valueType;};
    Type* getValueType() {return valueType;};
    std::string toStr();
    static PointerType* get(Type *valueType);
};

class TypeSystem
{
private:
    static IntType commonInt;
    static IntType commonBool;
    static VoidType commonVoid;
public:
    static Type *intType;
    static Type *voidType;
    static Type *boolType;
};

class ArrayType : public Type
{
private:
    Type *eleType;
    int numEle;
    struct HashEntry
    {
        ArrayType *arrayType;
        HashEntry *next;
    };
    static HashEntry* hashTable[256];
    static int hashFunc(Type *eleType, int numEle) {return ((unsigned long long)eleType + numEle) % 256;};
public:
    ArrayType(Type*eleType, int numEle) : Type(Type::ARRAY) {this->eleType = eleType; this->numEle = numEle;}
    Type* getEleType() {return eleType;};
    int getEleNum() {return numEle;};
    static ArrayType* get(Type *eleType, int numEle);
    std::string toStr();
};

#endif
