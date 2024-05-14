cmake_minimum_required(VERSION 3.26)
project(e2)

set(CMAKE_CXX_STANDARD 20)

add_executable(e2
                ast_printer.cpp ast_printer.hh
                ast_visitor.cpp ast_visitor.hh
                exp.cpp exp.hh
                exp_parser.cpp exp_parser.hh
                exp_visitor_test.cpp
                ast_evaluator.cpp ast_evaluator.hh)
