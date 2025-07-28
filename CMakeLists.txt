cmake_minimum_required(VERSION 3.20)
project(B1rd)

set(CMAKER_C_STANDARD 11)

add_executable(b1rd
    Src/main.c
    Src/lexer.c
)
target_include_directories(b1rd PRIVATE Include)