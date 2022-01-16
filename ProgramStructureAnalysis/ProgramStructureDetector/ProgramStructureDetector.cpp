#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
//#include "LoopDetector.h"

using namespace llvm;
/*
 * This detects total number of loops, total number of outer loops, and number of back edges via
 * the LoopInfo Analysis.
 */
namespace {
    struct ProgramStructureDetector : public ModulePass  {
        static char ID;
        ProgramStructureDetector() : ModulePass(ID) {}

        int br_count = 0;


        virtual bool runOnModule(Module &M){
          for(Module::iterator F = M.begin(), E = M.end(); F!= E; ++F){
            functionName(F);
            getLoopInfo(*F);
            traverseOuterLoop(*F);
            for(Function::iterator BB = F->begin(); F->end(); ++BB){
              ProgramStructureDetector::runOnBasicBlock(BB, M.getContext());
            }
          }
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

        virtual bool runOnBasicBlock(Function::iterator &BB, LLVMContext &context)
        {
          for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI) {
            int opcode = BI->getOpcode();
            if (opcode == 2) {
              br_count++;
              errs() << "Current br Count: "<< br_count << '\n';
            }
          }
          return true;
        }
    };
}

char ProgramStructureDetector::ID = 0;
static RegisterPass<ProgramStructureDetector> X("loopDetector", "LoopDetector Pass", false, false);