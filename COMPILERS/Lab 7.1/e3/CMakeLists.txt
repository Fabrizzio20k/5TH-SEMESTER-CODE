cmake_minimum_required(VERSION 3.26)
project(e3)

set(CMAKE_CXX_STANDARD 20)

add_executable(e3
                imp.cpp imp.hh
                imp_ast_test.cpp imp_ast_visitor.hh
                imp_interpreter.cpp imp_interpreter.hh
                imp_parser.cpp imp_parser.hh
                imp_printer.cpp imp_printer.hh)