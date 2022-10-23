cmake_minimum_required(VERSION 3.23)
project(DORTH C)

set(CMAKE_C_STANDARD 99)

EXECUTE_PROCESS(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
message(STATUS "Architecture: ${ARCHITECTURE}")
set(CMAKE_OSX_ARCHITECTURES ${ARCHITECTURE})

add_compile_options(-DCLION)
if (NOT (APPLE AND ${ARCHITECTURE} STREQUAL "arm64"))
    add_compile_options(-DHAVE_FFSL -DCAPSTONE_HAS_X86 -DDISABLE_DISASM)
else()
    add_compile_options(-DHAVE_FFSL -DCAPSTONE_HAS_ARM64 -DDISABLE_DISASM)
endif()

add_executable(DORTH
        main.c

        lightning/lightning_orig.c lightning/jit_memory.c lightning/jit_disasm.c
        lightning/jit_size.c lightning/jit_note.c lightning/jit_print.c prompt/prompt.c prompt/prompt.h)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND xxd -i -n Dorth ${PROJECT_SOURCE_DIR}/Dorth > ${PROJECT_SOURCE_DIR}/Dorth.c)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach (dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach ()