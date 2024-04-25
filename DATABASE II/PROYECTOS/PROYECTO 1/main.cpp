#include <iostream>
#include "sqlparser/SQLParser.h"

using namespace std;

int main() {
    const std::string query = "CREATE TABLE test (a INT, b INT);";
    hsql::SQLParserResult result;
    hsql::SQLParser::parse(query, &result);

    if (result.isValid() && result.size() > 0) {
        const hsql::SQLStatement* statement = result.getStatement(0);

        //print the statement type
        switch (statement->type()) {
            case hsql::kStmtSelect:
                cout << "SELECT statement" << endl;
                break;
            case hsql::kStmtImport:
                cout << "IMPORT statement" << endl;
                break;
            case hsql::kStmtInsert:
                cout << "INSERT statement" << endl;
                break;
            case hsql::kStmtUpdate:
                cout << "UPDATE statement" << endl;
                break;
            case hsql::kStmtDelete:
                cout << "DELETE statement" << endl;
                break;
            case hsql::kStmtCreate:
                cout << "CREATE statement" << endl;
                break;
            case hsql::kStmtDrop:
                cout << "DROP statement" << endl;
                break;
            case hsql::kStmtPrepare:
                cout << "PREPARE statement" << endl;
                break;
            case hsql::kStmtExecute:
                cout << "EXECUTE statement" << endl;
                break;
            case hsql::kStmtExport:
                cout << "EXPORT statement" << endl;
                break;
            case hsql::kStmtRename:
                cout << "RENAME statement" << endl;
                break;
            case hsql::kStmtAlter:
                cout << "ALTER statement" << endl;
                break;
            case hsql::kStmtShow:
                cout << "SHOW statement" << endl;
                break;
            case hsql::kStmtTransaction:
                cout << "TRANSACTION statement" << endl;
                break;
            default:
                cout << "Unknown statement" << endl;
                break;
        }
    }
    return 0;
}
