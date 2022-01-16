#include <fstream>
#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Assembly/Writer.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/CommandLine.h"

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

        void traverseOuterLoop(Function & F) {
          char outfile[256];
          std::sprintf(outfile, "loop_InfoInAllLoop.txt");
          std::ofstream ofs;
          ofs.open(outfile, std::ios::app);
          ofs << F.getName().str() << "\n";

          const LoopInfo *LI = &getAnalysis<LoopInfo>();
          errs() <<"Function" << F.getName() + "";
          for (Loop::iterator L= LI->begin(); L!=LI->end(); ++L) {
            getBBInfoInAllLoop(*L, 0, &ofs);
          }
          ofs.close();
        }

        void getBBInfoInAllLoop(Loop *L, unsigned nest, std::ofstream *ofs)  {
          (*ofs)<<"Loop ID: "<<toStringRef(L->getLoopID()).str()<<std::endl;
          (*ofs) << "Loop level " << nest << std::endl;

          unsigned num_Blocks = 0;
          Loop::block_iterator bb;
          for (bb = L->block_begin(); bb != L->block_end(); ++bb) {
            errs()<<"Basicblock ID: " << (*bb)->getValueID() << "\n";
            (*ofs)<<"Basicblock LLFI Index: ";
            for(BasicBlock::iterator BI = (*bb)->begin(), BE= (*bb)->end(); BI != BE; ++BI ) {
              long llfiIndex = getLLFIIndexofInst(BI);
              errs() << "llfi index: " << llfiIndex << "\n";
              (*ofs)<<llfiIndex<<"\n";
            }
            num_Blocks++;
          }

          (*ofs)<<"This Loop has" << num_Blocks
                 << " block\n";

          errs() <<"Loop Header Info Label " ;
          WriteAsOperand(errs(), L->getHeader(), false);
          errs() << "Loop level " << nest << " has " << num_Blocks
                   << " block\n";

          std::vector<Loop *> subLoops = L->getSubLoops();
          Loop::iterator j, f;
          for (j = subLoops.begin(), f = subLoops.end(); j != f; ++j) {
            getBBInfoInAllLoop(*j, nest + 1, ofs);
          }
        }


        bool functionName(Function *F) {
            errs().write_escaped(F->getName()) << '\n';
            return false;
        }

        long getLLFIIndexofInst(Instruction *inst) {
          MDNode *mdnode = inst->getMetadata("llfi_index"); //Todo: getMetadata
          if (mdnode) {
            ConstantInt *cns_index = dyn_cast<ConstantInt>(mdnode->getOperand(0)); //Todo: getOprand
            return cns_index->getSExtValue();
          }
          return 0;
        }

    };
}

char GetBBInfoInAllLoop::ID = 0;
static RegisterPass<GetBBInfoInAllLoop> X("GetBBLLFIInfoInAllLoop", "GetBBInfoLLFIInAllLoop Pass", false, false);