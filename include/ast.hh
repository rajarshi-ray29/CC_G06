#ifndef AST_HH
#define AST_HH

#include <llvm/IR/Value.h>
#include <string>
#include <vector>

struct LLVMCompiler;

/**
Base node class. Defined as `abstract`.
*/
struct Node
{
    enum NodeType
    {
        BIN_OP,
        INT_LIT,
        STMTS,
        ASSN,
        LET,
        DBG,
        IDENT,
        TERNARY,
        IF
    } type;
    std::string data_type;
    virtual std::string to_string() = 0;
    virtual llvm::Value *llvm_codegen(LLVMCompiler *compiler) = 0;
};

/**
    Node for list of statements
*/
struct NodeStmts : public Node
{
    std::vector<Node *> list;

    NodeStmts();
    void push_back(Node *node);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};

/**
    Node for binary operations
*/
struct NodeBinOp : public Node
{
    enum Op
    {
        PLUS,
        MINUS,
        MULT,
        DIV
    } op;

    Node *left, *right;

    NodeBinOp(Op op, Node *leftptr, Node *rightptr);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};
/**
    Node for ternary operations
*/
struct NodeTernary : public Node
{
    enum Op
    {
        TERN_OP
    } op;

    Node *left, *right, *mid;

    NodeTernary(Op op, Node *leftptr, Node *rightptr, Node *midptr);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};

/**
    Node for if statements
*/
struct NodeIf : public Node
{
    Node *cond, *ifstmt, *elsestmt;

    NodeIf(Node *condptr, Node *ifptr, Node *elseptr);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};

/**
    Node for integer literals
*/
struct NodeInt : public Node
{
    long value;

    NodeInt(long val);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};



/**
    Node for variable assignments
*/
struct NodeAssn : public Node
{
    std::string identifier;
    Node *expression;

    NodeAssn(std::string id, Node *expr);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};

/*
    Node for Let statements
*/

struct NodeLet : public Node
{
    std::string identifier;
    Node *expression;

    NodeLet(std::string id, std::string daty, Node *expr);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};

/**
    Node for `dbg` statements
*/
struct NodeDebug : public Node
{
    Node *expression;

    NodeDebug(Node *expr);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};

/**
    Node for idnetifiers
*/
struct NodeIdent : public Node
{
    std::string identifier;

    NodeIdent(std::string ident);
    std::string to_string();
    llvm::Value *llvm_codegen(LLVMCompiler *compiler);
};

#endif