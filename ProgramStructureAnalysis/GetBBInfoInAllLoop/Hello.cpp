#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
/*
 * This detects total number of loops, total number of outer loops, and number of back edges via
 * the LoopInfo Analysis.
 */
namespace {
    struct GetBBInfoInAllLoop : public LoopInfo  {
        static char ID;
        GetBBInfoInAllLoop() : LoopInfo() {}

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
            getBBInfoInAllLoop(*L, 0);
          }
        }

        void getBBInfoInAllLoop(Loop *L, unsigned nest) const {
          unsigned num_Blocks = 0;
          Loop::block_iterator bb;
          for (bb = L->block_begin(); bb != L->block_end(); ++bb)
            errs() << "Loop level " << nest << " has " << num_Blocks
                   << " block\n";
          std::vector<Loop *> subLoops = L->getSubLoops();
          Loop::iterator j, f;
          for (j = subLoops.begin(), f = subLoops.end(); j != f; ++j) {
            getBBInfoInAllLoop(*j, nest + 1);
          }
        }


        bool functionName(Function *F) {
            errs().write_escaped(F->getName()) << '\n';
            return false;
        }
    };
}

char GetBBInfoInAllLoop::ID = 0;
static RegisterPass<GetBBInfoInAllLoop> X("GetBBInfoInAllLoop", "GetBBInfoInAllLoop Pass", false, false);