#include <cstdio>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
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

        void traverseOuterLoop(Function & F)  {
          char outfile[256];
          std::sprintf(outfile, "loop_InfoInOuterLoop.txt");
          std::ofstream ofs;
          ofs.open(outfile, std::ios::app);
          ofs << F.getName().str() << "\n";

          const LoopInfo *LI = &getAnalysis<LoopInfo>();
          errs() <<"Function" << F.getName() + "";
          for (Loop::iterator L= LI->begin(); L!=LI->end(); ++L) {
            getBBInfoInOuterLoop(*L, 0, &ofs);
          }
          ofs.close();
        }

        void getBBInfoInOuterLoop(Loop *L, unsigned nest, std::ofstream *ofs)  {
          unsigned num_Blocks = 0;


          (*ofs)<<"Loop ID: "<<toStringRef(L->getLoopID()).str()<<std::endl;

          for (Loop::block_iterator bb = L->block_begin(); bb != L->block_end(); ++bb) {
            errs()<<"Basicblock ID: " << (*bb)->getValueID() << "\n";
            (*ofs)<<"Basicblock LLFI Index: ";
            for(BasicBlock::iterator BI = (*bb)->begin(), BE= (*bb)->end(); BI != BE; ++BI ) {
              long llfiIndex = getLLFIIndexofInst(BI);
              errs() << "llfi index: " << llfiIndex << "\n";
              (*ofs)<<llfiIndex<<"\n";
            }
            num_Blocks++;
          }
          errs() <<"Loop Header Info Label " ;
          WriteAsOperand(errs(), L->getHeader(), false);
          errs()<< " Loop ID "<< L->getLoopID() <<" --- Loop level"<<nest<<" has "<<num_Blocks<<" blocks\n";
        };

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

char GetBBInfoInOuterLoop::ID = 0;
static RegisterPass<GetBBInfoInOuterLoop> X("GetBBLLFIInfoInOuterLoop", "GetBBLLFIInfoInOuterLoop Pass", false, false);