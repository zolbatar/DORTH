#include <iostream>
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LLVMRemarkStreamer.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/IPO.h"
#include "lld/Common/Driver.h"
#include "CompilerLLVM.h"

#ifdef __APPLE__

#include <sys/sysctl.h>

#endif
#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

CompilerLLVM::CompilerLLVM()
{
	SetupCapstone();
	LLVMInitializeX86Target();
	LLVMInitializeAArch64Target();

	LLVMInitializeX86TargetInfo();
	LLVMInitializeAArch64TargetInfo();

	LLVMInitializeX86TargetMC();
	LLVMInitializeAArch64TargetMC();

	LLVMInitializeX86AsmPrinter();
	LLVMInitializeAArch64AsmPrinter();

	LLVMInitializeX86AsmParser();
	LLVMInitializeAArch64AsmParser();
}

std::string MCPU = "native";

std::list<std::string> MAttrs;

static std::string getCPUArch()
{
#ifdef __APPLE__
	uint32_t cputype = 0;
	size_t size = sizeof(cputype);
	int res = sysctlbyname("hw.cputype", &cputype, &size, NULL, 0);
	if (res)
	{
		std::cout << "Get CPU type error: " << res << std::endl;
		exit(1);
	}
	else
	{
		if (cputype == 7)
		{
			return "x86-64";
		}
		else
		{
			return "aarch64";
		}
	}
#endif
#ifdef WINDOWS
	return "x86-64";
#endif
}

static std::string getCPUStr()
{
	// If user asked for the 'native' CPU, autodetect here. If autodection fails,
	// this will set the CPU to an empty string which tells the target to
	// pick a basic default.
	if (MCPU == "native")
		return std::string(llvm::sys::getHostCPUName());

	return MCPU;
}

static std::string getFeaturesStr()
{
	llvm::SubtargetFeatures Features;

	// If user asked for the 'native' CPU, we need to autodetect features.
	// This is necessary for x86 where the CPU might not support all the
	// features the autodetected CPU name lists in the target. For example,
	// not all Sandybridge processors support AVX.
	if (MCPU == "native")
	{
		llvm::StringMap<bool> HostFeatures;
		if (llvm::sys::getHostCPUFeatures(HostFeatures))
			for (auto& F: HostFeatures)
				Features.AddFeature(F.first(), F.second);
	}

	for (auto const& MAttr: MAttrs)
		Features.AddFeature(MAttr);

	return Features.getString();
}

void CompilerLLVM::SetupProfile(bool optimise, bool allow_end, std::string module, size_t stack_size, size_t data_size)
{
	// Optimisations
	llvm::CodeGenOpt::Level OLvl = llvm::CodeGenOpt::Default;
	this->optimise = optimise;
	this->allow_end = allow_end;
	if (optimise)
	{
		OLvl = llvm::CodeGenOpt::Aggressive;
	}

	// Options
	std::string CPUArch = getCPUArch();
	std::string CPUStr = getCPUStr();
	std::string FeaturesStr = getFeaturesStr();
	std::cout << "CPU Architecture: " << CPUArch << std::endl;
	std::cout << "CPU String: " << CPUStr << std::endl;
	std::cout << "CPU Features: " << FeaturesStr << std::endl;

	// Target
	llvm::TargetOptions Options;
	llvm::Triple TheTriple;
	TheTriple.setTriple(llvm::sys::getDefaultTargetTriple());
	std::string Error;
	const llvm::Target* TheTarget = llvm::TargetRegistry::lookupTarget(CPUArch, TheTriple, Error);
	if (Error.length() > 0)
	{
		std::cout << "LLVM Error code: " << Error << std::endl;
		exit(1);
	}

	Target = std::unique_ptr<llvm::TargetMachine>(TheTarget->createTargetMachine(
		TheTriple.getTriple(), CPUStr, FeaturesStr,
		Options, llvm::None, llvm::None, OLvl, true));
	if (Target == nullptr)
	{
		std::cout << "Couldn't allocate target machine\n";
		exit(1);
	}

	// LLVM Core stuff
	Context = std::make_unique<llvm::LLVMContext>();
	Module = std::make_unique<llvm::Module>(module, *Context);
	auto dl = Target->createDataLayout();
	Module.get()->setDataLayout(dl);

	// Types
	TypeNone = llvm::Type::getVoidTy(Module->getContext());
	TypeBit = llvm::Type::getInt1Ty(Module->getContext());
	TypeFloat = llvm::Type::getDoubleTy(Module->getContext());
	TypeInt = llvm::Type::getInt64Ty(Module->getContext());
	TypePtr = llvm::Type::getInt64PtrTy(Module->getContext());

	// Stack
	auto typ = llvm::ArrayType::get(TypeInt, stack_size);
	auto init = llvm::ConstantAggregateZero::get(typ);
	globals["~Stack"] = new llvm::GlobalVariable(*Module, typ, false, llvm::GlobalValue::InternalLinkage, init, "Stack");
	globals["~SP"] = new llvm::GlobalVariable(*Module, TypeInt, false, llvm::GlobalValue::InternalLinkage, llvm::ConstantInt::get(TypeInt, 0), "SP");

	// Data
	auto typ_ds = llvm::ArrayType::get(TypeInt, data_size);
	auto init_ds = llvm::ConstantAggregateZero::get(typ_ds);
	globals["~Data"] = new llvm::GlobalVariable(*Module, typ_ds, false, llvm::GlobalValue::InternalLinkage, init_ds, "Data");
	globals["~DP"] = new llvm::GlobalVariable(*Module, TypeInt, false, llvm::GlobalValue::InternalLinkage, llvm::ConstantInt::get(TypeInt, 0), "DP");

	// Temp registers
	globals["~R0"] = new llvm::GlobalVariable(*Module, TypeInt, false, llvm::GlobalValue::InternalLinkage, llvm::ConstantInt::get(TypeInt, 0), "R0");
	globals["~R1"] = new llvm::GlobalVariable(*Module, TypeInt, false, llvm::GlobalValue::InternalLinkage, llvm::ConstantInt::get(TypeInt, 0), "R1");

	// Builders
	func = CreateFunc("Implicit");
	ir = CreateBuilder("Implicit Builder", func);

	if (allow_end)
	{
		globals["~QuitRequested"] = new llvm::GlobalVariable(*Module, TypeInt, false,
			llvm::GlobalValue::InternalLinkage,
			llvm::ConstantInt::get(TypeInt, 0),
			"QuitRequested");
	}

	std::cout << "LLVM initialisation complete\n";
}

void CompilerLLVM::AddOptPasses(llvm::legacy::PassManagerBase& passes, llvm::legacy::FunctionPassManager& fnPasses)
{
	llvm::PassManagerBuilder builder;
	builder.OptLevel = 3;
	builder.OptLevel = 3;
	builder.SizeLevel = 0;
	builder.Inliner = llvm::createFunctionInliningPass(3, 0, false);
	builder.LoopVectorize = true;
	builder.SLPVectorize = true;
	builder.VerifyInput = true;
	Target->adjustPassManager(builder);
	builder.Inliner = llvm::createFunctionInliningPass(3, 0, false);
	builder.populateFunctionPassManager(fnPasses);
	builder.populateModulePassManager(passes);
}

void CompilerLLVM::OptimiseModule()
{
	llvm::legacy::PassManager passes;
	passes.add(new llvm::TargetLibraryInfoWrapperPass(Target->getTargetTriple()));
	passes.add(llvm::createTargetTransformInfoWrapperPass(Target->getTargetIRAnalysis()));

	llvm::legacy::FunctionPassManager fnPasses(Module.get());
	fnPasses.add(llvm::createTargetTransformInfoWrapperPass(Target->getTargetIRAnalysis()));

	AddOptPasses(passes, fnPasses);

	fnPasses.doInitialization();
	for (llvm::Function& func: *Module)
	{
		bool changed = fnPasses.run(func);
		if (changed)
		{
			std::cout << "Function '" << std::string(func.getName()) << "' optimised\n";
		}
	}
	fnPasses.doFinalization();

	passes.add(llvm::createVerifierPass());
	passes.run(*Module);
}

llvm::Function* CompilerLLVM::CreateFunc(std::string name)
{
	llvm::Type* ret = TypeNone;
	llvm::ArrayRef<llvm::Type*> types;
	auto ft = llvm::FunctionType::get(ret, types, false);
	auto func = llvm::Function::Create(llvm::FunctionType::get(ret, types, false),
		llvm::Function::ExternalLinkage,
		name,
		Module.get());
	return func;
}

llvm::IRBuilder<>* CompilerLLVM::CreateBuilder(std::string name, llvm::Function* func)
{
	auto builder = new llvm::IRBuilder<>(llvm::BasicBlock::Create(Module->getContext(), name, func));
	return builder;
}

void CompilerLLVM::IncStack()
{
	auto new_value = IR()->CreateAdd(GetSP(), llvm::ConstantInt::get(TypeInt, 1));
	IR()->CreateStore(new_value, SP());
}

void CompilerLLVM::DecStack()
{
	auto new_value = IR()->CreateSub(GetSP(), llvm::ConstantInt::get(TypeInt, 1));
	IR()->CreateStore(new_value, SP());
}

void CompilerLLVM::IncDP(llvm::Value* v)
{
	auto new_value = IR()->CreateAdd(GetSP(), v);
	IR()->CreateStore(new_value, SP());
}

void CompilerLLVM::FinishFunc()
{
	IR()->CreateRetVoid();
}

llvm::IRBuilder<>* CompilerLLVM::IR()
{
	return ir;
}

llvm::GlobalVariable* CompilerLLVM::DP()
{
	return globals["~DP"];
}

llvm::Value* CompilerLLVM::GetDP()
{
	return IR()->CreateLoad(TypeInt, globals["~DP"]);
}

llvm::GlobalVariable* CompilerLLVM::SP()
{
	return globals["~SP"];
}

llvm::Value* CompilerLLVM::GetSP()
{
	return IR()->CreateLoad(TypeInt, globals["~SP"]);
}

llvm::GlobalVariable* CompilerLLVM::Data()
{
	return globals["~Data"];
}

llvm::Value* CompilerLLVM::DataLoc()
{
	return IR()->CreateGEP(Data()->getValueType(), Data(), { llvm::ConstantInt::get(TypeInt, 0), IR()->CreateLoad(TypeInt, DP()) });
}

llvm::Value* CompilerLLVM::DataLocAddress(llvm::Value* v)
{
	return IR()->CreateGEP(Data()->getValueType(), Data(), { llvm::ConstantInt::get(TypeInt, 0), v });
}

llvm::GlobalVariable* CompilerLLVM::Stack()
{
	return globals["~Stack"];
}

llvm::Value* CompilerLLVM::StackLoc()
{
	return IR()->CreateGEP(Stack()->getValueType(), Stack(), { llvm::ConstantInt::get(TypeInt, 0), IR()->CreateLoad(TypeInt, SP()) });
}

llvm::GlobalVariable* CompilerLLVM::R0()
{
	return globals["~R0"];
}

llvm::Value* CompilerLLVM::GetR0()
{
	return IR()->CreateLoad(TypeInt, R0());
}

llvm::GlobalVariable* CompilerLLVM::R1()
{
	return globals["~R1"];
}

llvm::Value* CompilerLLVM::GetR1()
{
	return IR()->CreateLoad(TypeInt, R1());
}

llvm::GlobalVariable* CompilerLLVM::CreateGlobal(std::string name)
{
	// Set at HERE, i.e. current data position
	auto gv = new llvm::GlobalVariable(*Module,
		TypePtr,
		false,
		llvm::GlobalValue::InternalLinkage,
		llvm::ConstantPointerNull::get(llvm::PointerType::get(TypePtr, 0)),
		name);
	globals[name] = gv;
	IR()->CreateStore(DataLoc(), gv);
	return globals[name];
}

llvm::GlobalVariable* CompilerLLVM::GetGlobal(std::string name)
{
	return globals[name];
}
