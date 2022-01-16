#include <cstdio>
#include <fstream>
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
    struct GetBBInfoInOuterLoop : public LoopInfo  {
        static char ID;
        GetBBInfoInOuterLoop() : LoopInfo() {}

        virtual bool runOnFunction(Function &F) {
          functionName(&F);

          traverseOuterLoop(F);
          return false;
        }

        virtual void getAnalysisUsage(AnalysisUsage &AU) const {
          AU.setPreservesAll();
          AU.addRequired<LoopInfo>();
        }

        void traverseOuterLoop(Function & F) const {
          const LoopInfo *LI = &getAnalysis<LoopInfo>();
          errs() <<"Function" << F.getName() + "";
          for (Loop::iterator L= LI->begin(); L!=LI->end(); ++L) {
            getBBInfoInOuterLoop(*L, 0);
          }
        }

        void getBBInfoInOuterLoop(Loop *L, unsigned nest) const {
          unsigned num_Blocks = 0;
          Loop::block_iterator bb;
          for(bb = L->block_begin(); bb!= L->block_end(); ++bb)
            num_Blocks++;
          errs() <<"Loop Header Info Label " ;
          WriteAsOperand(errs(), L->getHeader(), false);
          errs()<<" Loop ID "<< L->getLoopID() <<" --- Loop level"<<nest<<" has "<<num_Blocks<<" blocks\n";
        };

        bool functionName(Function *F) {
          errs().write_escaped(F->getName()) << '\n';
          return false;
        }


    };
}

char GetBBInfoInOuterLoop::ID = 0;
static RegisterPass<GetBBInfoInOuterLoop> X("GetBBInfoInOuterLoop", "GetBBInfoInOuterLoop Pass", false, false);