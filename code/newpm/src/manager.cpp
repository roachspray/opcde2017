/*
 * Pass Manager Implementation, divorcing itself from the
 * the legacy pass manager.
 *
 *
 */

#include "llvm/LinkAllPasses.h"
#include "llvm/AsmParser/Parser.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm-c/Core.h"

#include <iostream>
#include <fstream>

using namespace llvm;

#include "Analysis/TargetCallSitesPass.h"

/*
 * The only pass that is merged into this manager is ta simple function list utility
 * so there isn't a need for output, etc, but for consistency's sake it's best to keep 
 * a similar syntax.
 */
cl::opt<std::string> InputBitcodeFile(cl::Positional, cl::desc("<input.bc>"),
				      cl::Required);

/*
 * Utility for testing 
 */
std::unique_ptr<Module> parseIR(LLVMContext &Context, const char *IR) {
  SMDiagnostic Err;
  return parseAssemblyString(IR, Err, Context);
}


int main(int argc, char *argv[]) {

  std::error_code ec;
  // Here we can explicitly manage analyses, not just a pass.
  // in the grand scheme this saves us time (tends-to) as analysis
  // passes don't transform the IR.  More importantly it allows us to 
  // customize the analysis output!
  SMDiagnostic err;
  std::unique_ptr<Module> irModule;

  cl::ParseCommandLineOptions( argc, argv );

  outs() << "Reading bitcode from file: " << InputBitcodeFile << '\n';

  irModule = parseIRFile( InputBitcodeFile, err, *unwrap(LLVMGetGlobalContext()));
  if ( irModule == nullptr ) {
    errs() << "Error: " << err.getMessage().str() << '\n';
    return -1;
  }

  int FunctionPassCount = 0;
  int AnalyzedInstrCount = 0;
  int AnalyzedFunctionCount = 0;

  FunctionPassManager FPM( true );
  FPM.addPass( TestFunctionPass(FunctionPassCount, AnalyzedInstrCount, AnalyzedFunctionCount,true) );
  
  // run our analysis through as a pass
  ModulePassManager MPM(true);
  MPM.addPass( createModuleToFunctionPassAdaptor(std::move(FPM)) );

  // create a function analysis manager
  FunctionAnalysisManager FAM(true);
  int FunctionAnalysisRuns = 0;
  FAM.registerPass([&] { return TestFunctionAnalysis(FunctionAnalysisRuns); } );

  // create our module analysis manager and register the available passes
  ModuleAnalysisManager MAM( true );
  MAM.registerPass( [&] { return FunctionAnalysisManagerModuleProxy(FAM); });

  MPM.run( *irModule.get(), MAM );
  outs() << "Functions analyzed: " << AnalyzedFunctionCount << '\n';
  outs() << "Instructions analyzed: " << AnalyzedInstrCount << '\n';

  // je suis fine...   
  return 0;

}
