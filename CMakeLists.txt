cmake_minimum_required(VERSION 3.20)
project(DORTH)

if (APPLE)
    add_compile_options(-Wno-deprecated-declarations)
endif ()

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(${PROJECT_NAME}
        main.cpp
        ImGuiBindings/imgui_impl_sdl.cpp ImGuiBindings/imgui_impl_opengl3.cpp)
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})
