/*
 * This is a dependency for a few of the passes. Collects
 * call sites and organizes them by TargetCallType.
 *
 */
 
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CallSite.h"
#include "llvm/Support/raw_ostream.h"

#include <string>
#include <iostream>
#include <fstream>

using namespace llvm;

#include "TestFunctionPass.h"


