#ifndef __TARGETCALLSITESPASS_H
#define	__TARGETCALLSITESPASS_H

#include "llvm/IR/PassManager.h"

using namespace llvm;

namespace {

class TestFunctionAnalysisPass : public AnalysisInfoMixin<TestFunctionAnalysisPass> {
 public:
  struct Result {
    Result(int Count) : InstructionCount(Count) {}
    int InstructionCount;
  };

  TestFunctionAnalysisPass(int &Runs) : Runs(Runs) {}

  Result run(Function &F, FunctionAnalysisManager &AM)
  {
    ++Runs;
    int Count = 0;
    for ( Instruction &I : instructions(F) ) {
      ++Count;
    }
    return TestFunctionAnalysisPass::Result(Count);
  }
  

 private:
  friend AnalysisInfoMixin<TestFunctionAnalysisPass>;
  static AnalysisKey Key;
  int &Runs;

};

  AnalysisKey TestFunctionAnalysisPass::Key;
}

#endif
