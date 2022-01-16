#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Assembly/Writer.h"

using namespace llvm;
/*
 * This detects total number of loops, total number of outer loops, and number of back edges via
 * the LoopInfo Analysis.
 */
namespace {
    struct GetBBName : public FunctionPass  {
        static char ID;
        GetBBName() : FunctionPass(ID) {}

        virtual bool runOnFunction(Function &F) {
          functionName(&F);
          for(Function::iterator BB = F.begin(); BB != F.end(); ++BB){
            errs() <<"BB Label " ;
            WriteAsOperand(errs(),BB, false);
            errs() << " BB Size " << BB->size() << " BB valueID " << BB->getValueID() << " BB value Name "  << BB->getValueName() <<" BB Name " << BB->getName()  <<"  \n";
          }
          return false;
        }

        bool functionName(Function *F) {
          errs().write_escaped(F->getName()) << '\n';
          return false;
        }
    };
}

char GetBBName::ID = 0;
static RegisterPass<GetBBName> X("GetBBName", "GetBBName Pass", false, false);