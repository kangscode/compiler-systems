cmake_minimum_required(VERSION 3.16)
project(tinyc_llvm LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(tinyc
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/codegen.cpp
)

target_include_directories(tinyc PRIVATE include)

enable_testing()
add_test(NAME smoke
    COMMAND tinyc ${CMAKE_CURRENT_SOURCE_DIR}/examples/arith.tiny
            -o ${CMAKE_CURRENT_BINARY_DIR}/arith.ll)
