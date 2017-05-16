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

  /// \brief Run the analysis pass over the function and return a result.
  Result run(Function &F, FunctionAnalysisManager &AM) {
    ++Runs;
    int Count = 0;
    for (Function::iterator BBI = F.begin(), BBE = F.end(); BBI != BBE; ++BBI)
      for (BasicBlock::iterator II = BBI->begin(), IE = BBI->end(); II != IE;
           ++II)
        ++Count;
    return Result(Count);
  }

private:
  friend AnalysisInfoMixin<TestFunctionAnalysis>;
  static AnalysisKey Key;

  int &Runs;
};

AnalysisKey TestFunctionAnalysis::Key;

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

struct TestModulePass : PassInfoMixin<TestModulePass> {
  TestModulePass(int &RunCount) : RunCount(RunCount) {}

  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    ++RunCount;
    return PreservedAnalyses::none();
  }

  int &RunCount;
};

struct TestPreservingModulePass : PassInfoMixin<TestPreservingModulePass> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    return PreservedAnalyses::all();
  }
};

struct TestFunctionPass : PassInfoMixin<TestFunctionPass> {
  TestFunctionPass(int &RunCount, int &AnalyzedInstrCount,
                   int &AnalyzedFunctionCount,
                   bool OnlyUseCachedResults = false)
      : RunCount(RunCount), AnalyzedInstrCount(AnalyzedInstrCount),
        AnalyzedFunctionCount(AnalyzedFunctionCount),
        OnlyUseCachedResults(OnlyUseCachedResults) {}

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    ++RunCount;

    const ModuleAnalysisManager &MAM =
        AM.getResult<ModuleAnalysisManagerFunctionProxy>(F).getManager();
    if (TestModuleAnalysis::Result *TMA =
            MAM.getCachedResult<TestModuleAnalysis>(*F.getParent()))
      AnalyzedFunctionCount += TMA->FunctionCount;

    if (OnlyUseCachedResults) {
      // Hack to force the use of the cached interface.
      if (TestFunctionAnalysis::Result *AR =
              AM.getCachedResult<TestFunctionAnalysis>(F))
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
