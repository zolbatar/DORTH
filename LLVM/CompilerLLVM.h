#pragma once
#include <memory>
#include <map>
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/IRBuilder.h"

struct FuncBuilder
{
	llvm::Function* func;
	llvm::IRBuilder<>* builder;
};

class CompilerLLVM
{
 public:
	CompilerLLVM();
	void SetupProfile(bool optimise, bool allow_end, std::string module, size_t stack_size, size_t data_size);
	llvm::Function* CreateFunc(std::string name);
	llvm::IRBuilder<>* CreateBuilder(std::string name, llvm::Function* func);
	void FinishFunc();
	void Disassemble(void* exec, size_t sz, size_t address);
	llvm::GlobalVariable* CreateGlobal(std::string name);

	void Run();

	llvm::Type* TypeNone = nullptr;
	llvm::Type* TypeBit = nullptr;
	llvm::Type* TypeInt = nullptr;
	llvm::Type* TypePtr = nullptr;
	llvm::Type* TypeFloat = nullptr;
	std::unique_ptr<llvm::Module> Module = nullptr;
	std::unique_ptr<llvm::LLVMContext> Context = nullptr;

	llvm::GlobalVariable* GetGlobal(std::string name);
	llvm::IRBuilder<>* IR();

	// Stack
	llvm::GlobalVariable* SP();
	llvm::Value* GetSP();
	llvm::GlobalVariable* Stack();
	llvm::Value* StackLoc();
	void IncStack();
	void DecStack();

	// Data
	llvm::GlobalVariable* DP();
	llvm::Value* GetDP();
	llvm::GlobalVariable* Data();
	llvm::Value* DataLoc();
	llvm::Value* DataLocAddress(llvm::Value* v);
	void IncDP(llvm::Value* v);

	// Forth system registers
	llvm::GlobalVariable* R0();
	llvm::Value* GetR0();
	llvm::GlobalVariable* R1();
	llvm::Value* GetR1();
 private:
	void AddOptPasses(llvm::legacy::PassManagerBase& passes, llvm::legacy::FunctionPassManager& fnPasses);
	void OptimiseModule();
	void SetupCapstone();

	bool optimise;
	bool allow_end;

	std::unique_ptr<llvm::TargetMachine> Target = nullptr;
	std::map<std::string, llvm::GlobalVariable*> globals;
	llvm::Function* func = nullptr;
	llvm::IRBuilder<>* ir;

};
