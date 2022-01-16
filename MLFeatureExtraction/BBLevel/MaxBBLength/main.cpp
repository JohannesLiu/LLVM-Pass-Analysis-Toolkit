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

      errs() << "Function name: " << F->getName() << ".\n";
      if(F->getName() == "printCounter" || F->getName() == "counterAdd"){
        continue;
      }

      if(F->getName() == "main"){
        errs() << "Execute at the end of main function!\n";

        Function::iterator BB = F->getEntryBlock();
        BasicBlock::iterator BI = BB->end();

        LLVMContext& context = appModule->getContext();

        FunctionType* counterAddType = FunctionType::get(Type::getVoidTy(context), false); //Function Type
        Constant* counterAdd_handler_c = appModule->getOrInsertFunction("printMaxResult", counterAddType); //Function Handler
        Function *counterAddFunc = dyn_cast<Function>(counterAdd_handler_c); //Function without params

        Instruction *newInst = CallInst::Create(counterAddFunc,  ""); //Instruction
        BB->getInstList().insert((*BI).getPrevNode(), newInst);
        errs() << "Inserted the printCounter function at the end of main function!\n";
      }

      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
      {


        errs() << "Clean Local MaxMin Result!\n";
        //Add by xiaokai
        LLVMContext& context = appModule->getContext();

        FunctionType* resetMinMaxType = FunctionType::get(Type::getVoidTy(context), false); //Function Type
        Constant* resetMinMax_handler_c = appModule->getOrInsertFunction("resetLocalResult", resetMinMaxType); //Function Handler
        Function *resetMinMaxFunc = dyn_cast<Function>(resetMinMax_handler_c); //Function

        Instruction *newResetMinMax = CallInst::Create(resetMinMaxFunc, ""); //Instruction
        BB->getInstList().insert(BB->end()->getPrevNode(), newResetMinMax);
        errs() << "Inserted the function!\n";

        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI) {
          errs() << "Update Local MaxMin Result!\n";
          // Add by xiaokai

          FunctionType *updateLocalMaxType = FunctionType::get(
              Type::getVoidTy(context), false); // Function Type
          Constant *updateLocalMax_handler_c = appModule->getOrInsertFunction(
              "updateLocalMaxResult", updateLocalMaxType); // Function Handler
          Function *updateLocalMaxFunc =
              dyn_cast<Function>(updateLocalMax_handler_c); // Function

          Instruction *newUpdateLocalMaxInst =
              CallInst::Create(updateLocalMaxFunc, ""); // Instruction
          BB->getInstList().insert(BB->end()->getPrevNode(), newUpdateLocalMaxInst);
          errs() << "Inserted the function!\n";
        }

        errs() << "Clean Local MaxMin Result!\n";
        //Add by xiaokai

        FunctionType* updateGlobalMaxType = FunctionType::get(Type::getVoidTy(context), false); //Function Type
        Constant* updateGlobalMax_handler_c = appModule->getOrInsertFunction("resetLocalResult", updateGlobalMaxType); //Function Handler
        Function *updateGlobalMaxFunc = dyn_cast<Function>(updateGlobalMax_handler_c); //Function

        Instruction *newUpdateGlobalMax = CallInst::Create(updateGlobalMaxFunc, ""); //Instruction
        BB->getInstList().insert(BB->end()->getPrevNode(), newUpdateGlobalMax);
        errs() << "Inserted the function!\n";
      }

    }

    return true;
  }
};
char ir_instrumentation::ID = 0;
static RegisterPass<ir_instrumentation> X("ir-instrumentation", "LLVM IR Instrumentation Pass");

}