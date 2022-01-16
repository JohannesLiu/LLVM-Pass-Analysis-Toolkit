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
    struct LoopDetector : public LoopInfo  {
        static char ID;
        LoopDetector() : LoopInfo() {}

        virtual bool runOnFunction(Function &F) {
            functionName(&F);
            getLoopInfo(F);
            return false;
        }

        virtual void getAnalysisUsage(AnalysisUsage &AU) const {
            AU.setPreservesAll();
            AU.addRequired<LoopInfo>();
        }

        void getLoopInfo(const Function& F) const {
            const LoopInfo *LI = &getAnalysis<LoopInfo>();
            Function::const_iterator BB = F.begin(), E = F.end();
            int numOfTotalLoops = 0;
            int curLevel = 0;
            int numOfOuterLoops = 0;
            int prevLevel = 0;
            int numOfBackEdges = 0;
            for(; BB != E; ++BB) {
                curLevel = LI->getLoopDepth(&*BB);
                if (curLevel > prevLevel) numOfTotalLoops++;
                if (prevLevel == 0 && curLevel == 1) numOfOuterLoops++;
                if (prevLevel > curLevel) numOfBackEdges++;
                prevLevel = curLevel;
            }
            errs() << "Total Loops: " << numOfTotalLoops << '\n';
            errs() << "Total Outer Loops: " << numOfOuterLoops << '\n';
            errs() << "Total Loop Exit CFG Edges: " << numOfBackEdges << '\n';
        };

        bool functionName(Function *F) {
            errs().write_escaped(F->getName()) << '\n';
            return false;
        }
    };
}

char LoopDetector::ID = 0;
static RegisterPass<LoopDetector> X("loopDetector", "LoopDetector Pass", false, false);