//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "hello"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/InstIterator.h"
#include <llvm/Support/CommandLine.h>

#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include<sstream>
#include<algorithm>
#include <set>
#include <vector>

using namespace llvm;

static cl::opt<std::string> cl_select_set_file_name("select_set_file", cl::desc("Specify the file of selected sets"), cl::value_desc("file name"));


namespace{



/****** analysis pass ********/
struct ProfileCounter : public ModulePass{
  static char ID;
  Function *hook;
  std::vector<long> selectedIndexInsts;

  ProfileCounter() : ModulePass(ID) {}

  virtual bool runOnModule(Module &M)
  {
    Constant *hookFunc;
    hookFunc = M.getOrInsertFunction("calculateExecCount", Type::getVoidTy(M.getContext()), Type::getInt64Ty(M.getContext()), NULL);

    hook= cast<Function>(hookFunc);

    for(Module::iterator F = M.begin(), E = M.end(); F!= E; ++F)
    {
      //errs() << "Function: " << F->getName() << '\n';
      std::string functionName = F->getName();

      if(functionName != "calculateExecCount" && !F->isDeclaration()){
        // std::string func_name = F->getName();
        // errs() <<  "ff func name: " << func_name << "\n";
        for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
        {
          ProfileCounter::runOnBasicBlock(BB, M.getContext());
        }

      }


    }

    //errs() << "Add Basic Block Specific Checkpoint" << '\n';
    addEndProfilingFuncCall(M);
    return false;
  }
  virtual bool runOnBasicBlock(Function::iterator &BB, LLVMContext &context)
  {

    for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI)
    {
      //errs() << BI->getOpcode() << "\n";
      //errs() << BI->getOpcodeName() << "\n";
      long llfiIndex = getLLFIIndexofInst(BI);
      int opcode = BI->getOpcode();
      std::vector<long>::iterator ret;
      ret = std::find(selectedIndexInsts.begin(), selectedIndexInsts.end(), llfiIndex);
      if(llfiIndex > 0 && opcode != 1 && opcode != 2 && opcode != 28 && opcode != 48 && opcode != 59 && ret != selectedIndexInsts.end() ){
        //errs() << BI->getOpcodeName() << "\n";
        Value* indexValue = ConstantInt::get(Type::getInt64Ty(context), llfiIndex);
        CallInst* call_print = CallInst::Create(hook, indexValue, "");
        BB->getInstList().insert(BI, call_print);
        break;
      }

    }


    return true;
  }

  void addEndProfilingFuncCall(Module &M) {
    Function* mainfunc = M.getFunction("main");
    if (mainfunc != NULL) {
      Constant *endprofilefunc = getLLFILibEndProfilingFunc(M);

      // function call
      std::set<Instruction*> exitinsts;
      getProgramExitInsts(M, exitinsts);
      assert (exitinsts.size() != 0
             && "Program does not have explicit exit point");

      for (std::set<Instruction*>::iterator it = exitinsts.begin();
           it != exitinsts.end(); ++it) {
        Instruction *term = *it;
        CallInst::Create(endprofilefunc, "", term);
      }
    } else {
      errs() << "ERROR: Function main does not exist, " <<
          "which is required by analysis-pass\n";
      exit(1);
    }
  }

  Constant *getLLFILibEndProfilingFunc(Module &M) {
    LLVMContext& context = M.getContext();
    FunctionType* endprofilingfunctype = FunctionType::get(Type::getVoidTy(context), false);
    Constant *endprofilefunc = M.getOrInsertFunction("calculateAndDumpResult",
                                                     endprofilingfunctype);
    return endprofilefunc;
  }

  void getProgramExitInsts(Module &M, std::set<Instruction*> &exitinsts) {
    for (Module::iterator m_it = M.begin(); m_it != M.end(); ++m_it) {
      if (!m_it->isDeclaration()) {
        //m_it is a function
        for (inst_iterator f_it = inst_begin(m_it); f_it != inst_end(m_it);
             ++f_it) {
          Instruction *inst = &(*f_it);
          if (CallInst *ci = dyn_cast<CallInst>(inst)) {
            Function *calledFunc = ci->getCalledFunction();
            if (calledFunc && calledFunc->hasName() &&
                calledFunc->getName().str() == "exit") {
              exitinsts.insert(inst);
            }
          }
        }
      }
    }

    Function* mainfunc = M.getFunction("main");
    exitinsts.insert(getTermInstofFunction(mainfunc));
  }

  Instruction *getTermInstofFunction(Function *func) {
    BasicBlock &termbb = func->back();
    Instruction *ret = termbb.getTerminator();

    assert(isa<ReturnInst>(ret) || isa<UnreachableInst>(ret) &&
                                       "Last instruction is not return or exit() instruction");
    return ret;
  }

  long getLLFIIndexofInst(Instruction *inst) {
    MDNode *mdnode = inst->getMetadata("llfi_index");
    if (mdnode) {
      ConstantInt *cns_index = dyn_cast<ConstantInt>(mdnode->getOperand(0));
      return cns_index->getSExtValue();
    }
    return 0;
  }

  void readSelectSet()
  {
    std::ifstream select_set_file;
    select_set_file.open(cl_select_set_file_name.c_str()); //Todo: What the file is it? A file consists of a series of numbers?
    if(!select_set_file.is_open())
    {
      errs()<<"\nERROR: can not open select_set_file!\n";
      exit(1);
    }

    while(select_set_file.good())
    {
      std::string line;
      getline(select_set_file, line);
      if(line.empty())	continue;
      else
      {
        long currentIndex = atol(line.c_str());
        //errs() << "Select Index: " << currentIndex << "\n";
        selectedIndexInsts.push_back(currentIndex);
      }
    }
    //std::sort(selected_insts.begin(), selected_insts.end());
  }
};

}

char ProfileCounter::ID = 0;
static RegisterPass<ProfileCounter> X("exec_counter", "execution count", false, false);
