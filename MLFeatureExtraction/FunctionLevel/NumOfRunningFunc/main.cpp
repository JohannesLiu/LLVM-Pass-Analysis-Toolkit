#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/IRBuilder.h"

#include <vector>

using namespace llvm;

namespace{
struct ir_instrumentation : public ModulePass{
  static char ID;
  Function *monitor;
  Module *appModule;

  ir_instrumentation() : ModulePass(ID) {}

  virtual bool runOnModule(Module &M)
  {
    errs() << "====----- Entered Module " << M.getModuleIdentifier() << " -----====.\n";

    appModule = &M;

    for(Module::iterator F = M.begin(), E = M.end(); F!= E; ++F)
    {
      if(isa<Function>(&(*F))){
        Function::iterator BB = F->getEntryBlock();
        BasicBlock::iterator BI = BB->end();

        LLVMContext& context = appModule->getContext();
        FunctionType* counterAddType = FunctionType::get(Type::getVoidTy(context), false); //Function Type
        Constant* counterAdd_handler_c = appModule->getOrInsertFunction("printCounter", counterAddType); //Function Handler
        Function *counterAddFunc = dyn_cast<Function>(counterAdd_handler_c); //Function without params

        Instruction *newInst = CallInst::Create(counterAddFunc,  ""); //Instruction
        BB->getInstList().insert((*BI).getPrevNode(), newInst);
        errs() << "Inserted the printCounter function at the end of main function!\n";
      }
    }
    return true;
  }
};
char ir_instrumentation::ID = 0;
static RegisterPass<ir_instrumentation> X("ir-instrumentation", "LLVM IR Instrumentation Pass");

}