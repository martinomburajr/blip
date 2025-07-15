#include "llvm/IR/PassManager.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassBuilder.h"     // ✅ add this
#include "llvm/Passes/PassPlugin.h"      // ✅ and this
#include "llvm/IR/Instructions.h"
#include <cstdint>

using namespace llvm;

/// Performs computation of the stack allocations done at compile time
uint64_t compute_stack_alloc(Function &F) {
    vector<*AllocaInst> allocs;
    
    uint64_t total_alloc_size = 0;
    
    // Get the first address of the first allocation if any;
    // uintptr_t address_start;
    
    // Iterate over the basic blocks to retrieve all AllocAInst
    for (auto& BB: F) {
        for (auto& I: BB) {
            if (isa<AllocaInst>(I)) {
                allocs.push_back(I)
            }
        }
    }
  
    // Iterate over the allocations
    for (auto& alloc: allocs) {
        if (alloc.isArrayAllocation()) {
            // Wont work as getArraySize() returns a Value* not a number
            total_alloc_size += alloc.getArraySize();
        } else {
            total_alloc_size += alloc.getAllocationSize();
        }
    }
    
    return total_alloc_size;
}

namespace {
    struct FunctionProfilerPass: PassInfoMixin<FunctionProfilerPass> {
        PreservedAnalyses run(Function &F, FunctionAnalysisManager&) {
            if (F.isDeclaration())
                return PreservedAnalyses::all();
                
            Module *M = F.getParent();
            LLVMContext &Ctx = M->getContext();
            
            // We declare the Builder variable using constructor initialization
            IRBuilder<> Builder(&*F.getEntryBlock().getFirstInsertionPt());
            
            // Declare the functions in our profiler code.
            FunctionCallee startFn = M->getOrInsertFunction(
                "start_timer", FunctionType::get(Type::getVoidTy(Ctx), false));
            
            FunctionCallee endFn = M->getOrInsertFunction(
                "end_timer", 
                FunctionType::get(Type::getVoidTy(Ctx), 
                { PointerType::getUnqual(Type::getInt8Ty(Ctx)) }, 
                false)
            );
            
            // Start the timer by calling "start_timer"
            Builder.CreateCall(startFn);
            
            // Get the name of the function that is being accessed;
            IRBuilder<> GlobalBuilder(M->getContext());
            auto *funcName = Builder.CreateGlobalString(F.getName());
            
            // Inject end_timer("func_name") before the return
            for (auto& BB: F) {
                for (auto& I: BB) {
                    if (isa<ReturnInst>(I)) {
                        IRBuilder<> RetBuilder(&I);
                        RetBuilder.CreateCall(endFn, {funcName});
                    }
                }
            }
            
            return PreservedAnalyses::none();
        }
    };
};

extern "C" ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
      LLVM_PLUGIN_API_VERSION, "FunctionProfilerPass", "v0.1",
      [](PassBuilder &PB) {
        PB.registerPipelineParsingCallback(
            [](StringRef Name, FunctionPassManager &FPM,
               ArrayRef<PassBuilder::PipelineElement>) {
              if (Name == "function-profiler") {
                FPM.addPass(FunctionProfilerPass());
                return true;
              }
              return false;
            });
      }};
}
