cmake_minimum_required(VERSION 3.25)
project(untitled2)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled2
        test_imp_fun.cpp
        imp.cpp
        imp_parser.cpp
        imp_printer.cpp
        imp_interpreter.cpp
        imp_value.cpp
        imp_typechecker.cpp
        imp_type.cpp
        )
