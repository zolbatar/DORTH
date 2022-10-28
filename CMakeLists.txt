cmake_minimum_required(VERSION 3.23)
project(DORTH)

set(CMAKE_CXX_STANDARD 20)

EXECUTE_PROCESS(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
message(STATUS "Architecture: ${ARCHITECTURE}")
set(CMAKE_OSX_ARCHITECTURES ${ARCHITECTURE})

include_directories(${CMAKE_SOURCE_DIR}/capstone/include)

add_compile_options(-DCLION)
if (NOT (APPLE AND ${ARCHITECTURE} STREQUAL "arm64"))
    #-DDISABLE_DISASM
    add_compile_options(-DHAVE_FFSL -DCAPSTONE_HAS_X86 -DRICH)
else ()
    add_compile_options(-DHAVE_FFSL -DCAPSTONE_HAS_ARM64 -DRICH)
endif ()

include(${CMAKE_SOURCE_DIR}/cmake-build-${CMAKE_BUILD_TYPE}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(DORTH
        main.cpp

        compiler/compiler.c
        compiler/compiler.h
        compiler/stack.c
        compiler/stack.h
        compiler/native.c
        compiler/disassemble.c
        compiler/word.c

        lightning/lightning_orig.c
        lightning/jit_memory.c
        lightning/jit_disasm.c
        lightning/jit_size.c
        lightning/jit_note.c
        lightning/jit_print.c

        capstone/arch/X86/X86Disassembler.c
        capstone/arch/X86/X86IntelInstPrinter.c
        capstone/arch/X86/X86ATTInstPrinter.c
        capstone/arch/X86/X86Mapping.c
        capstone/arch/X86/X86Module.c
        capstone/arch/X86/X86DisassemblerDecoder.c

        capstone/arch/AArch64/AArch64BaseInfo.c
        capstone/arch/AArch64/AArch64Disassembler.c
        capstone/arch/AArch64/AArch64InstPrinter.c
        capstone/arch/AArch64/AArch64Mapping.c
        capstone/arch/AArch64/AArch64Module.c

        capstone/cs.c
        capstone/MCInst.c
        capstone/MCInstrDesc.c
        capstone/MCRegisterInfo.c
        capstone/SStream.c
        capstone/utils.c

        rich/keyboard.c
        rich/keyboard.h
        rich/prompt.cpp
        rich/UI/UISDL.cpp
        rich/console/Console.cpp

        ImGuiBindings/imgui_impl_opengl3.cpp
        ImGuiBindings/imgui_impl_sdl.cpp compiler/optimiser.c)
target_link_libraries(${PROJECT_NAME} ${LIBS} ${CONAN_LIBS})
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND xxd -i -n Dorth ${PROJECT_SOURCE_DIR}/Dorth > ${PROJECT_SOURCE_DIR}/Dorth.c)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach (dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach ()