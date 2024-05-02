#include <sstream>
#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include "prueba4.hh"
#include "prueba4_parser.hh."

int main(int argc, const char* argv[]) {
  Program *program;
  cout << "Reading program from file " << argv[1] << endl;
  std::ifstream inputFile(argv[1]);
  stringstream buffer;
  string line;
  buffer << inputFile.rdbuf();
  line = buffer.str();
  Scanner scanner(line.c_str());
  Parser parser(&scanner);
  program = parser.parse();
  cout << "program: " << endl;
  program->print();
  cout << endl;
  cout << "execute: " << endl;
  program->execute();
  delete program;
}
