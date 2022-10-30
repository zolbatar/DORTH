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

void CompilerLLVM::SetupProfile(bool optimise, bool allow_end, std::string module, size_t stack_size)
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

/*	Module->getOrInsertFunction("PrintByte", TypeNone, TypeByte);
	Module->getOrInsertFunction("PrintInteger", TypeNone, TypeInt);
	Module->getOrInsertFunction("PrintFloat", TypeNone, TypeFloat);
	Module->getOrInsertFunction("PrintString", TypeNone, TypeString);
	Module->getOrInsertFunction("PrintByteFormat", TypeNone, TypeByte, TypeString);
	Module->getOrInsertFunction("PrintIntegerFormat", TypeNone, TypeInt, TypeString);
	Module->getOrInsertFunction("PrintFloatFormat", TypeNone, TypeFloat, TypeString);
	Module->getOrInsertFunction("PrintStringFormat", TypeNone, TypeString, TypeString);
	Module->getOrInsertFunction("PrintNewline", TypeNone);
	Module->getOrInsertFunction("Strings_AddPermanent", TypeNone, TypeString);*/

	// Stack
	auto typ = llvm::ArrayType::get(TypeInt, stack_size);
	auto init = llvm::ConstantAggregateZero::get(llvm::ArrayType::get(TypeInt, stack_size));
	globals["~Stack"] = new llvm::GlobalVariable(*Module, typ, false, llvm::GlobalValue::InternalLinkage, init, "Stack");
	globals["~SP"] = new llvm::GlobalVariable(*Module, TypeInt, false, llvm::GlobalValue::InternalLinkage, llvm::ConstantInt::get(TypeInt, 0), "SP");
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

void CompilerLLVM::IncStack(size_t v)
{
	auto new_value = IR()->CreateAdd(SP(), llvm::ConstantInt::get(TypeInt, v));
	IR()->CreateStore(new_value, SP());
}

void CompilerLLVM::DecStack(size_t v)
{
	auto new_value = IR()->CreateSub(SP(), llvm::ConstantInt::get(TypeInt, v));
	IR()->CreateStore(new_value, SP());
}

void CompilerLLVM::FinishFunc()
{
	IR()->CreateRetVoid();
}
