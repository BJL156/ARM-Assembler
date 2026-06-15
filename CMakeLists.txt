cmake_minimum_required(VERSION 3.10)
project(assembler C)
set(CMAKE_C_STANDARD 11)

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra)
endif()

file(GLOB SOURCES "src/*.c")
add_executable(assembler ${SOURCES})
target_include_directories(assembler PRIVATE src)
