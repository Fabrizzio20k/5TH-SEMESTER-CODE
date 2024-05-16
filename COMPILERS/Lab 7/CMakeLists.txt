cmake_minimum_required(VERSION 3.28)
project(Lab_7)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Lab_7
        environment.cpp
        environment.hh
        imp.cpp
        imp.hh
        imp_interpreter.cpp
        imp_interpreter.hh
        imp_parser.cpp
        imp_parser.hh
        imp_printer.cpp
        imp_printer.hh
        imp_test.cpp
        imp_visitor.hh)
