#include "SymbolTable.h"
#include "Type.h"
#include <iostream>
#include <sstream>

SymbolEntry::SymbolEntry(Type *type, int kind) 
{
    this->type = type;
    this->kind = kind;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type, int value) : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    this->value = value;
}

std::string ConstantSymbolEntry::toStr()
{
    std::ostringstream buffer;
    buffer << value;
    return buffer.str();
}

IdentifierSymbolEntry::IdentifierSymbolEntry(Type *type, std::string name, int scope, bool is_extern) : SymbolEntry(type, SymbolEntry::VARIABLE), name(name)
{
    this->scope = scope;
    initVal = 0;
    this->is_extern = is_extern;
    addr = nullptr;
}

std::string IdentifierSymbolEntry::toStr()
{
    if(isGlobal())
        return "@" + name;
    return "%" + name;
}

std::string IdentifierSymbolEntry::getInitValStr() 
{
    std::ostringstream buffer;
    buffer << initVal;
    return buffer.str();
}

TemporarySymbolEntry::TemporarySymbolEntry(Type *type, int label) : SymbolEntry(type, SymbolEntry::TEMPORARY)
{
    this->label = label;
}

std::string TemporarySymbolEntry::toStr()
{
    std::ostringstream buffer;
    buffer << "%t" << label;
    return buffer.str();
}

SymbolTable::SymbolTable()
{
    prev = nullptr;
    level = 0;
}

SymbolTable::SymbolTable(SymbolTable *prev)
{
    this->prev = prev;
    this->level = prev->level + 1;
}

/*
    Description: lookup the symbol entry of an identifier in the symbol table
    Parameters: 
        name: identifier name
    Return: pointer to the symbol entry of the identifier

    hint:
    1. The symbol table is a stack. The top of the stack contains symbol entries in the current scope.
    2. Search the entry in the current symbol table at first.
    3. If it's not in the current table, search it in previous ones(along the 'prev' link).
    4. If you find the entry, return it.
    5. If you can't find it in all symbol tables, return nullptr.
*/
SymbolEntry* SymbolTable::lookup(std::string name)
{
    for(SymbolTable *st = this; st != nullptr; st = st->prev)
    {
        auto it = st->symbolTable.find(name);
        if(it != st->symbolTable.end())
            return it->second;
    }
    return nullptr;
}

void SymbolTable::init() 
{
    Type *i32Ty = IntType::get(32);
    Type *voidTy = VoidType::get();
    Type *ptrToI32Ty = PointerType::get(i32Ty);

    Type *funcTy;
    IdentifierSymbolEntry *func;
    std::string name;

    name = "getint";
    funcTy = FunctionType::get(i32Ty, {});
    func = new IdentifierSymbolEntry(funcTy, name, globals->getLevel(), true);
    globals->install(name, func);

    name = "getch";
    funcTy = FunctionType::get(i32Ty, {});
    func = new IdentifierSymbolEntry(funcTy, name, globals->getLevel(), true);
    globals->install(name, func);
    
    name = "getarray";
    funcTy = FunctionType::get(i32Ty, {ptrToI32Ty});
    func = new IdentifierSymbolEntry(funcTy, name, globals->getLevel(), true);
    globals->install(name, func);

    name = "putint";
    funcTy = FunctionType::get(voidTy, {i32Ty});
    func = new IdentifierSymbolEntry(funcTy, name, globals->getLevel(), true);
    globals->install(name, func);

    name = "putch";
    funcTy = FunctionType::get(voidTy, {i32Ty});
    func = new IdentifierSymbolEntry(funcTy, name, globals->getLevel(), true);
    globals->install(name, func);

    name = "putarray";
    funcTy = FunctionType::get(voidTy, {i32Ty, ptrToI32Ty});
    func = new IdentifierSymbolEntry(funcTy, name, globals->getLevel(), true);
    globals->install(name, func);

    name = "_sysy_starttime";
    funcTy = FunctionType::get(voidTy, {i32Ty});
    func = new IdentifierSymbolEntry(funcTy, name, globals->getLevel(), true);
    globals->install(name, func);

    name = "_sysy_stoptime";
    funcTy = FunctionType::get(voidTy, {i32Ty});
    func = new IdentifierSymbolEntry(funcTy, name, globals->getLevel(), true);
    globals->install(name, func);
}

// install the entry into current symbol table.
void SymbolTable::install(std::string name, SymbolEntry* entry)
{
    symbolTable[name] = entry;
}

int SymbolTable::counter = 0;
SymbolTable SymbolTable::t;
SymbolTable * SymbolTable::identifiers = &t;
SymbolTable * SymbolTable::globals = &t;