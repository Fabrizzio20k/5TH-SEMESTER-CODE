#include <sstream>
#include <iostream>
#include <iostream>

#include "imp.hh"
#include "imp_parser.hh"

int main(int argc, const char *argv[]) {
    bool useparser = true;
    Program *program;

    if (useparser) {

        if (argc != 2) {
            cout << "Incorrect number of arguments" << endl;
            exit(1);
        }

        Scanner scanner(argv[1]);
        Parser parser(&scanner);
        program = parser.parse();  // el parser construye el AST de Program

    } else {
        program = new Program();
        Exp *e = new BinaryExp(new NumberExp(2), new NumberExp(3), EXP);
        Stm *s1 = new AssignStatement("x", e);
        Stm *s2 = new PrintStatement(new IdExp("x"));
        program->add(s1);
        program->add(s2);

    }

    cout << "program: " << endl;
    program->print();
    cout << endl;

    cout << "execute: " << endl;
    program->execute();

    delete program;


}
