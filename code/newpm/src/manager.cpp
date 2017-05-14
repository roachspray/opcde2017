/*
 * Pass Manager Implementation, divorcing itself from the
 * the legacy pass manager.
 *
 *
 */

#include "llvm/LinkAllPasses.h"
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
cl::opt<std::string> OutputBitcodeFile(cl::Positional, cl::desc("<output.bc>"),
				       cl::Required);

cl::opt<bool> FunctionRecognition("function-recognition",cl::init(false));


int main(int argc, char *argv[]) {

  std::error_code ec;
  FunctionAnalysisManager FAM;
  SMDiagnostic err;
  std::unique_ptr<Module> irModule;
  raw_fd_ostream *outputStream;

  cl::ParseCommandLineOptions( argc, argv );

  outs() << "Reading bitcode from file: " << InputBitcodeFile << '\n';
  irModule = parseIRFile( InputBitcodeFile, err, *unwrap(LLVMGetGlobalContext()));
  if ( irModule == nullptr ) {
    errs() << "Error: " << err.getMessage().str() << '\n';
    return -1;
  }

  if ( FunctionRecognition ) {
    int Runs = 0;
    FAM.registerPass( [&] { return TestFunctionAnalysisPass(Runs); });

  }

  // je suis fine...
   
  return 0;

}
