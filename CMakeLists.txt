cmake_minimum_required(VERSION 3.20)
project(DORTH)

if (APPLE)
    add_compile_options(-Wno-deprecated-declarations)
endif ()

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# LLVM
if (WIN32)
    set(LLVM_DIR "C:/Users/d_dud/Dev/llvm-13.0.0.src")
    include_directories(${LLVM_DIR}/include)
    #set(LLVM_BUILD_DIR "M:/dev/llvm")
    set(LLVM_BUILD_DIR "C:/Users/d_dud/Dev/llvm")
    include_directories(${LLVM_BUILD_DIR}/include)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        link_directories(${LLVM_BUILD_DIR}/Release/lib)
    else ()
        link_directories(${LLVM_BUILD_DIR}/Release/lib)
    endif ()
endif ()
Set(LLVM_LIBS_WIN
        LLVMAnalysis.lib
        LLVMCore.lib;
        LLVMExecutionEngine.lib
        LLVMInstCombine.lib
        LLVMObject.lib
        LLVMOrcShared.lib
        LLVMOrcTargetProcess.lib
        LLVMRuntimeDyld.lib
        LLVMScalarOpts.lib
        LLVMSupport.lib
        LLVMX86CodeGen.lib
        LLVMX86AsmParser.lib
        LLVMX86Desc.lib
        LLVMX86Disassembler.lib
        LLVMX86Info.lib
        LLVMJITLink.lib
        LLVMOrcShared.lib
        LLVMPasses.lib
        LLVMCoroutines.lib
        LLVMipo.lib
        LLVMInstrumentation.lib
        LLVMVectorize.lib
        LLVMFrontendOpenMP.lib
        LLVMIRReader.lib
        LLVMAsmParser.lib
        LLVMLinker.lib
        LLVMAsmPrinter.lib
        LLVMDebugInfoDWARF.lib
        LLVMCFGuard.lib
        LLVMGlobalISel.lib
        LLVMSelectionDAG.lib
        LLVMCodeGen.lib
        LLVMAggressiveInstCombine.lib
        LLVMTarget.lib
        LLVMTransformUtils.lib
        LLVMBitWriter.lib
        LLVMBitReader.lib
        LLVMProfileData.lib
        LLVMRemarks.lib
        LLVMBitstreamReader.lib
        LLVMTextAPI.lib
        LLVMMC.lib
        LLVMMCParser.lib
        LLVMMCDisassembler.lib
        LLVMBinaryFormat.lib
        LLVMDebugInfoCodeView.lib
        LLVMDebugInfoMSF.lib
        LLVMDemangle.lib)

if (APPLE)
    set(LLVM_DIR "/usr/local/opt/llvm@13")
    include_directories(${LLVM_DIR}/include)
    link_directories(${LLVM_DIR}/lib)
endif ()
Set(LLVM_LIBS_MAC
        libLLVMAnalysis.a
        libLLVMCore.a;
        libLLVMExecutionEngine.a
        libLLVMInstCombine.a
        libLLVMObject.a
        libLLVMOrcJIT.a
        libLLVMRuntimeDyld.a
        libLLVMScalarOpts.a
        libLLVMSupport.a
        libLLVMARMCodeGen.a
        libLLVMARMAsmParser.a
        libLLVMARMDesc.a
        libLLVMARMDisassembler.a
        libLLVMARMInfo.a
        libLLVMX86CodeGen.a
        libLLVMX86AsmParser.a
        libLLVMX86Desc.a
        libLLVMX86Disassembler.a
        libLLVMX86Info.a
        libLLVMJITLink.a
        libLLVMOrcShared.a
        libLLVMOrcTargetProcess.a
        libLLVMPasses.a
        libLLVMCoroutines.a
        libLLVMipo.a
        libLLVMInstrumentation.a
        libLLVMVectorize.a
        libLLVMFrontendOpenMP.a
        libLLVMIRReader.a
        libLLVMAsmParser.a
        libLLVMLinker.a
        libLLVMAsmPrinter.a
        libLLVMDebugInfoDWARF.a
        libLLVMCFGuard.a
        libLLVMGlobalISel.a
        libLLVMSelectionDAG.a
        libLLVMCodeGen.a
        libLLVMAggressiveInstCombine.a
        libLLVMTarget.a
        libLLVMTransformUtils.a
        libLLVMBitWriter.a
        libLLVMBitReader.a
        libLLVMProfileData.a
        libLLVMRemarks.a
        libLLVMBitstreamReader.a
        libLLVMTextAPI.a
        libLLVMMC.a
        libLLVMMCParser.a
        libLLVMMCDisassembler.a
        libLLVMBinaryFormat.a
        libLLVMDebugInfoCodeView.a
        libLLVMDebugInfoMSF.a
        libLLVMDemangle.a
        libLLVMAArch64AsmParser.a
        libLLVMAArch64CodeGen.a
        libLLVMAArch64Desc.a
        libLLVMAArch64Disassembler.a
        libLLVMAArch64Info.a
        libLLVMAArch64Utils.a)

add_executable(${PROJECT_NAME}
        main.cpp
        ImGuiBindings/imgui_impl_sdl.cpp ImGuiBindings/imgui_impl_opengl3.cpp JIT/JIT.h JIT/JIT.cpp
        JIT/JIT.cpp JIT/WOFAlloc.cpp)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} libz.a libncurses.tbd ${LLVM_LIBS_MAC})
endif ()
if (WIN32)
    target_link_libraries(${PROJECT_NAME} ${LLVM_LIBS_WIN})
endif ()

target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})
