
#define DEBUG_TYPE "hello"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/InstIterator.h"

#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <set>

using namespace llvm;

namespace{
  /****** analysis pass ********/
  struct GetBranchIndex : public ModulePass{
    static char ID;
    int br_count = 0;
    GetBranchIndex() : ModulePass(ID) {}

      virtual bool runOnModule(Module &M){
        for(Module::iterator F = M.begin(), E = M.end(); F!= E; ++F){
          for(Function::iterator BB = F->begin(); F->end(); ++BB){
            GetBranchIndex::runOnBasicBlock(BB, M.getContext());
          }
        }
        return false;
      }

      virtual bool runOnBasicBlock(Function::iterator &BB, LLVMContext &context)
      {
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI) {
          int opcode = BI->getOpcode();
          if (opcode == 2) {
            br_count++;
            errs() << br_count << '\n';
          }
        }
        return true;
      }
  };
}

char GetBranchIndex::ID = 0;
static RegisterPass<GetBranchIndex> X("CallCount", "call count", false, false);