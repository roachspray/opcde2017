
# Attempt to leverage the new LLVM Pass Manager

The code here is intended to be some basics that help with learning
the LLVM API. The points dealt with here are:

- Pass manager use
- Pass dependency

This sample is somewhat extracted to create an isolated learning from the
LLVM source.  For those interested in the new pass manager please check
out Chandler's talk in 2014 at the LLVM Developer's meeting where he
explains a lot of the rationale behind the design decisions.

This is just for those curious as to how passes might be linked together and
especially regarding analysis passes in the future.  The legacy pass manager,
at the time of writing (LLVM 4.0) is still the dominant use pattern...

Again, just a learning tool for those interested in where pass management is
heading.

## A Sample run 

With all of the logging turned on
```
$  ./build/bin/Manager build/tests/FE001.bc 
Reading bitcode from file: build/tests/FE001.bc
warning: ignoring debug info with an invalid version (700000003) in build/tests/FE001.bc
Running analysis: InnerAnalysisManagerProxy<FunctionAnalysisManager, llvm::Module>
Starting llvm::Function pass manager run.
Running pass: (anonymous namespace)::TestFunctionPass on FE001_foo
Running analysis: OuterAnalysisManagerProxy<ModuleAnalysisManager, llvm::Function>
Running analysis: (anonymous namespace)::TestFunctionAnalysis
Finished llvm::Function pass manager run.
Starting llvm::Function pass manager run.
Running pass: (anonymous namespace)::TestFunctionPass on main
Running analysis: OuterAnalysisManagerProxy<ModuleAnalysisManager, llvm::Function>
Running analysis: (anonymous namespace)::TestFunctionAnalysis
Finished llvm::Function pass manager run.
Functions analyzed: 2
Instructions analyzed: 8
```