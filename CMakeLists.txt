cmake_minimum_required(VERSION 3.22)

project(RISC-V-SIMULATOR)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(code
    src/Memory.cpp
    src/Decoder.cpp
    src/Register.cpp
    src/RegisterFile.cpp
    src/ALU.cpp
    src/RS.cpp
    src/ROB.cpp
    src/IQ.cpp
    src/LSB.cpp
    src/CPU.cpp
    src/main.cpp
)