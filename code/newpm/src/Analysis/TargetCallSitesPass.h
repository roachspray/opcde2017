#ifndef __TARGETCALLSITESPASS_H
#define	__TARGETCALLSITESPASS_H

#include "llvm/IR/PassManager.h"

using namespace llvm;

namespace {

class TestFunctionAnalysis : public AnalysisInfoMixin<TestFunctionAnalysis> {
 public:
  struct Result {
    Result(int Count) : InstructionCount(Count) {}
    int InstructionCount;
  };

  TestFunctionAnalysis(int &Runs) : Runs(Runs) {}

  Result run(Function &F, FunctionAnalysisManager &AM)
  {
    outs() << "Counting instructions in " << F.getName() << '\n';
    ++Runs;
    int Count = 0;
    for ( Instruction &I : instructions(F) ) {
      (void)I;
      ++Count;
    }
    return TestFunctionAnalysis::Result(Count);
  }
  

 private:
  friend AnalysisInfoMixin<TestFunctionAnalysis>;
  static AnalysisKey Key;
  int &Runs;

  };

  AnalysisKey TestFunctionAnalysis::Key;

  // Module analysis
  class TestModuleAnalysis : public AnalysisInfoMixin<TestModuleAnalysis> {
  public:
    struct Result {
    Result(int Count) : FunctionCount(Count) {}
      int FunctionCount;
    };

  TestModuleAnalysis(int &Runs) : Runs(Runs) {}

    Result run(Module &M, ModuleAnalysisManager &AM) {
      ++Runs;
      int Count = 0;
      for (Module::iterator I = M.begin(), E = M.end(); I != E; ++I)
	++Count;
      return Result(Count);
    }

  private:
    friend AnalysisInfoMixin<TestModuleAnalysis>;
    static AnalysisKey Key;

    int &Runs;
  };

  AnalysisKey TestModuleAnalysis::Key;

  
  // struct for the function pass...
  struct TestFunctionPass : PassInfoMixin<TestFunctionPass> {
  TestFunctionPass(int &RunCount, int &AnalyzedInstrCount, int &AnalyzedFunctionCount, bool OnlyUseCachedResults = false) : RunCount(RunCount),
      AnalyzedInstrCount(AnalyzedInstrCount), AnalyzedFunctionCount(AnalyzedFunctionCount), OnlyUseCachedResults(OnlyUseCachedResults) {}

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
      ++RunCount;

      const ModuleAnalysisManager &MAM = AM.getResult<ModuleAnalysisManagerFunctionProxy>(F).getManager();
      if ( TestModuleAnalysis::Result *TMA = MAM.getCachedResult<TestModuleAnalysis>(*F.getParent()))
	AnalyzedFunctionCount += TMA->FunctionCount;
      
      if (OnlyUseCachedResults) {
	if ( TestFunctionAnalysis::Result * AR = AM.getCachedResult<TestFunctionAnalysis>(F) )
	  AnalyzedInstrCount += AR->InstructionCount;
      } else {
	// Typical path just runs the analysis as needed.                                                                                  
	TestFunctionAnalysis::Result &AR = AM.getResult<TestFunctionAnalysis>(F);
	AnalyzedInstrCount += AR.InstructionCount;
      }
      return PreservedAnalyses::all();
    }

    int &RunCount;
    int &AnalyzedInstrCount;
    int &AnalyzedFunctionCount;
    bool OnlyUseCachedResults;

  };

}

#endif
