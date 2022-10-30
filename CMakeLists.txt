cmake_minimum_required(VERSION 3.23)
project(DORTH)

set(CMAKE_CXX_STANDARD 20)

EXECUTE_PROCESS(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
message(STATUS "Architecture: ${ARCHITECTURE}")
set(CMAKE_OSX_ARCHITECTURES ${ARCHITECTURE})
include_directories(${CMAKE_SOURCE_DIR}/Capstone/include)

if (NOT (APPLE AND ${ARCHITECTURE} STREQUAL "arm64"))
    add_compile_options(-DSDL -DHAVE_FFSL -DCAPSTONE_HAS_X86)
else ()
    add_compile_options(-DSDL -DHAVE_FFSL -DCAPSTONE_HAS_ARM64)
endif ()

include(${CMAKE_SOURCE_DIR}/cmake-build-${CMAKE_BUILD_TYPE}/conanbuildinfo.cmake)
conan_basic_setup()

# LLVM
if (${ARCHITECTURE} STREQUAL "arm64")
    set(CORE_LLVM_DIR /opt/homebrew/Cellar/llvm/15.0.2/)
else ()
    set(CORE_LLVM_DIR /usr/local/Cellar/llvm/15.0.2/)
endif ()
set(LLVM_DIR "${CORE_LLVM_DIR}lib/cmake/llvm")
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
llvm_map_components_to_libnames(LLVM_LIBS core support WebAssembly AArch64 X86 Coroutines OrcJIT)

add_executable(DORTH
        main.cpp

        Compiler/Compiler.cpp
        Compiler/Compiler.h
        Compiler/Tokens.h
        Compiler/Words.h
        Compiler/Native.cpp
        LLVM/Disassembler.cpp
        Compiler/Expand.cpp
        Compiler/DumpIR.cpp
        Compiler/Word.cpp

        LLVM/CompilerLLVM.cpp
        LLVM/CompilerLLVM.h
        LLVM/Run.cpp

        JIT/JIT.cpp
        JIT/JIT.h

        Library/StringLib.cpp
        Library/StringLib.h

        Capstone/arch/X86/X86Disassembler.c
        Capstone/arch/X86/X86IntelInstPrinter.c
        Capstone/arch/X86/X86ATTInstPrinter.c
        Capstone/arch/X86/X86Mapping.c
        Capstone/arch/X86/X86Module.c
        Capstone/arch/X86/X86DisassemblerDecoder.c

        Capstone/arch/AArch64/AArch64BaseInfo.c
        Capstone/arch/AArch64/AArch64Disassembler.c
        Capstone/arch/AArch64/AArch64InstPrinter.c
        Capstone/arch/AArch64/AArch64Mapping.c
        Capstone/arch/AArch64/AArch64Module.c

        Capstone/cs.c
        Capstone/MCInst.c
        Capstone/MCInstrDesc.c
        Capstone/MCRegisterInfo.c
        Capstone/SStream.c
        Capstone/utils.c

        UI/UISDL.cpp
        Console/Console.cpp

        ImGuiBindings/imgui_impl_opengl3.cpp
        ImGuiBindings/imgui_impl_sdl.cpp

        Runtime/Console.cpp
        Runtime/Runtime.cpp Runtime/Runtime.h)
target_link_libraries(${PROJECT_NAME} ${LIBS} ${CONAN_LIBS} ${LLVM_LIBS})

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach (dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach ()