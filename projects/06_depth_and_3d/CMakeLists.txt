cmake_minimum_required(VERSION 3.16)
project(QtOpenGL06 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)

add_executable(opengl_06 main.cpp smoke_test.h)
target_link_libraries(opengl_06 PRIVATE Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(opengl_06 PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(CTest)
if(BUILD_TESTING)
    add_test(NAME opengl_06_smoke COMMAND opengl_06 --smoke-test)
    set_tests_properties(opengl_06_smoke PROPERTIES TIMEOUT 20)
endif()
