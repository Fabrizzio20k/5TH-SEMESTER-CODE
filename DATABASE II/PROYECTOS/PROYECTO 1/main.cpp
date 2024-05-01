#include "sqlparser.h"

int main()
{
    SQLParser parser;
    parser.parse("create table students from file \"students.txt\" using AVL Index (\"id\")");
    parser.printSentence();

    parser.parse("select * from students where id = 123");
    parser.printSentence();

    parser.parse("select * from students where id between 123 and 456");
    parser.printSentence();

    parser.parse("insert into students values (123, \"pepe\", 20)");
    parser.printSentence();

    parser.parse("delete from students where id = 123");
    parser.printSentence();

    return 0;
}