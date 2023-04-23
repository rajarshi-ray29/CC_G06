#include "llvmcodegen.hh"
#include "ast.hh"
#include <iostream>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <vector>

#define MAIN_FUNC compiler->module.getFunction("main")

/*
The documentation for LLVM codegen, and how exactly this file works can be found
ins `docs/llvm.md`
*/

void LLVMCompiler::compile(Node *root)
{
    /* Adding reference to print_i in the runtime library */
    // void printi();
    FunctionType *printi_func_type = FunctionType::get(
        builder.getVoidTy(),
        {builder.getInt32Ty()},
        false);
    Function::Create(
        printi_func_type,
        GlobalValue::ExternalLinkage,
        "printi",
        &module);
    /* we can get this later
        module.getFunction("printi");
    */

    /* Main Function */
    // int main();
    FunctionType *main_func_type = FunctionType::get(
        builder.getInt32Ty(), {}, false /* is vararg */
    );
    Function *main_func = Function::Create(
        main_func_type,
        GlobalValue::ExternalLinkage,
        "main",
        &module);

    // create main function block
    BasicBlock *main_func_entry_bb = BasicBlock::Create(
        *context,
        "entry",
        main_func);

    // move the builder to the start of the main function block
    builder.SetInsertPoint(main_func_entry_bb);

    root->llvm_codegen(this);

    // return 0;
    builder.CreateRet(builder.getInt32(0));
}

void LLVMCompiler::dump()
{
    outs() << module;
}

void LLVMCompiler::write(std::string file_name)
{
    std::error_code EC;
    raw_fd_ostream fout(file_name, EC, sys::fs::OF_None);
    WriteBitcodeToFile(module, fout);
    fout.flush();
    fout.close();
}

//  ┌―――――――――――――――――――――┐  //
//  │ AST -> LLVM Codegen │  //
// └―――――――――――――――――――――┘   //

// codegen for statements
Value *NodeStmts::llvm_codegen(LLVMCompiler *compiler)
{
    Value *last = nullptr;
    for (auto node : list)
    {
        last = node->llvm_codegen(compiler);
    }

    return last;
}

Value *NodeDebug::llvm_codegen(LLVMCompiler *compiler)
{
    Value *expr = expression->llvm_codegen(compiler);

    Function *printi_func = compiler->module.getFunction("printi");
    compiler->builder.CreateCall(printi_func, {expr});

    return expr;
}

Value *NodeInt::llvm_codegen(LLVMCompiler *compiler)
{
    return compiler->builder.getInt32(value);
}

Value *NodeBinOp::llvm_codegen(LLVMCompiler *compiler)
{
    Value *left_expr = left->llvm_codegen(compiler);
    Value *right_expr = right->llvm_codegen(compiler);

    switch (op)
    {
    case PLUS:
        return compiler->builder.CreateAdd(left_expr, right_expr, "addtmp");
    case MINUS:
        return compiler->builder.CreateSub(left_expr, right_expr, "minustmp");
    case MULT:
        return compiler->builder.CreateMul(left_expr, right_expr, "multtmp");
    case DIV:
        return compiler->builder.CreateSDiv(left_expr, right_expr, "divtmp");
    }
}

Value *NodeTernary::llvm_codegen(LLVMCompiler *compiler)
{
    return nullptr;
}

Value *NodeIf::llvm_codegen(LLVMCompiler *compiler)
{
    Value *CondV = cond->llvm_codegen(compiler);
    if (!CondV)
        return nullptr;

    // Convert condition to a bool by comparing non-equal to 0.0.
    CondV = compiler->builder.CreateICmpNE(
        CondV, compiler->builder.getInt32(0), "ifcond");
    // builder.CreateRet(builder.getInt32(0));
    Function *TheFunction = compiler->builder.GetInsertBlock()->getParent();

     // end of the function.
    BasicBlock *ThenBB =
        BasicBlock::Create(*compiler->context, "then", TheFunction);
    BasicBlock *ElseBB = BasicBlock::Create(*compiler->context, "else");
    BasicBlock *MergeBB = BasicBlock::Create(*compiler->context, "ifcont");

    compiler->builder.CreateCondBr(CondV, ThenBB, ElseBB);

    // Emit then value.
    compiler->builder.SetInsertPoint(ThenBB);

    Value *ThenV = ifstmt->llvm_codegen(compiler);
    if (!ThenV)
    return nullptr;

    compiler->builder.CreateBr(MergeBB);
    // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
    ThenBB = compiler->builder.GetInsertBlock();

    // Emit else block.
    TheFunction->getBasicBlockList().push_back(ElseBB);
    
    compiler->builder.SetInsertPoint(ElseBB);

    Value *ElseV = elsestmt->llvm_codegen(compiler);
    if (!ElseV)
    return nullptr;

    compiler->builder.CreateBr(MergeBB);
    // codegen of 'Else' can change the current block, update ElseBB for the PHI.
    ElseBB = compiler->builder.GetInsertBlock();
    // Emit merge block.
    TheFunction->getBasicBlockList().push_back(MergeBB);
    
    compiler->builder.SetInsertPoint(MergeBB);
    PHINode *PN =
        compiler->builder.CreatePHI(compiler -> builder.getInt32Ty(), 2, "iftmp");

    PN->addIncoming(ThenV, ThenBB);
    PN->addIncoming(ElseV, ElseBB);
    return PN;

}

Value *NodeAssn::llvm_codegen(LLVMCompiler *compiler)
{
    Value *expr = expression->llvm_codegen(compiler);

    IRBuilder<> temp_builder(
        &MAIN_FUNC->getEntryBlock(),
        MAIN_FUNC->getEntryBlock().begin());

    AllocaInst *alloc = temp_builder.CreateAlloca(compiler->builder.getInt32Ty(), 0, identifier);

    compiler->locals[identifier] = alloc;

    return compiler->builder.CreateStore(expr, alloc);

}

Value *NodeLet::llvm_codegen(LLVMCompiler *compiler)
{
    Value *expr = expression->llvm_codegen(compiler);

    IRBuilder<> temp_builder(
        &MAIN_FUNC->getEntryBlock(),
        MAIN_FUNC->getEntryBlock().begin());

    AllocaInst *alloc = temp_builder.CreateAlloca(compiler->builder.getInt32Ty(), 0, identifier);

    compiler->locals[identifier] = alloc;

    return compiler->builder.CreateStore(expr, alloc);
}

Value *NodeIdent::llvm_codegen(LLVMCompiler *compiler)
{
    AllocaInst *alloc = compiler->locals[identifier];

    // if your LLVM_MAJOR_VERSION >= 14
    return compiler->builder.CreateLoad(compiler->builder.getInt32Ty(), alloc, identifier);
}

#undef MAIN_FUNC