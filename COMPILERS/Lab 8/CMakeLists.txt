cmake_minimum_required(VERSION 3.24)
project(Lab_8)

set(CMAKE_CXX_STANDARD 20)

add_executable(Lab_8 environment.cpp environment.hh
                imp.cpp imp.hh
                imp_interpreter.cpp imp_interpreter.hh
                imp_parser.cpp imp_parser.hh
                imp_printer.cpp imp_printer.hh
                imp_test.cpp imp_visitor.hh)