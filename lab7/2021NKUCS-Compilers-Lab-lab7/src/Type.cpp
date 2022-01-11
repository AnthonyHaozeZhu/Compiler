#include "Type.h"
#include <assert.h>
#include <sstream>


IntType TypeSystem::commonConstInt = IntType(32, true);
IntType TypeSystem::commonInt = IntType(32);
IntType TypeSystem::commonBool = IntType(1);
VoidType TypeSystem::commonVoid = VoidType();

Type* TypeSystem::constIntType = &commonConstInt;
Type* TypeSystem::intType = &commonInt;
Type* TypeSystem::voidType = &commonVoid;
Type* TypeSystem::boolType = &commonBool;
Type* TypeSystem::arrayType = &commonInt;

std::string IntType::toStr() {
    std::ostringstream buffer;
    if (constant)
        buffer << "i";
    else
        buffer << "i";
    buffer << size;
    return buffer.str();
}

std::string VoidType::toStr() {
    return "void";
}

std::string ArrayType::toStr() {
    std::vector<std::string> vec;
    Type* temp = this;
    int count = 0;
    bool flag = false;
    while (temp && temp->isArray()) {
        std::ostringstream buffer;
        if (((ArrayType*)temp)->getLength() == -1) {
            flag = true;
        } else {
            buffer << "[" << ((ArrayType*)temp)->getLength() << " x ";
            count++;
            vec.push_back(buffer.str());
        }
        temp = ((ArrayType*)temp)->getElementType();
    }
    assert(temp->isInt());
    std::ostringstream buffer;
    for (auto it = vec.begin(); it != vec.end(); it++)
        buffer << *it;
    buffer << "i32";
    while (count--)
        buffer << ']';
    if (flag)
        buffer << '*';
    return buffer.str();
}

std::string FunctionType::toStr() {
    std::ostringstream buffer;
    buffer << returnType->toStr() << "(";
    for (auto it = paramsType.begin(); it != paramsType.end(); it++) {
        buffer << (*it)->toStr();
        if (it + 1 != paramsType.end())
            buffer << ", ";
    }
    buffer << ')';
    return buffer.str();
}

std::string StringType::toStr() {
    std::ostringstream buffer;
    buffer << "const char[" << length << "]";
    return buffer.str();
}

std::string PointerType::toStr() {
    std::ostringstream buffer;
    buffer << valueType->toStr() << "*";
    return buffer.str();
}
