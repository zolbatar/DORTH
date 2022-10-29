cmake_minimum_required(VERSION 3.23)
project(DORTH)

set(CMAKE_CXX_STANDARD 20)

EXECUTE_PROCESS(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
message(STATUS "Architecture: ${ARCHITECTURE}")
set(CMAKE_OSX_ARCHITECTURES ${ARCHITECTURE})
include_directories(${CMAKE_SOURCE_DIR}/Capstone/include)
add_compile_options(-DHAVE_FFSL -DCapstone_HAS_X86 -DCapstone_HAS_ARM64)

include(${CMAKE_SOURCE_DIR}/cmake-build-${CMAKE_BUILD_TYPE}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(DORTH
        main.cpp

        Compiler/Compiler.cpp
        Compiler/Compiler.h
        Compiler/Tokens.h
        Compiler/Native.cpp
        Compiler/Disassembler.cpp

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
        Runtime/Runtime.cpp Compiler/Words.h Runtime/Runtime.h)
target_link_libraries(${PROJECT_NAME} ${LIBS} ${CONAN_LIBS})

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach (dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach ()