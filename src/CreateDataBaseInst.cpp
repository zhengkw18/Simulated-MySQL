#include "Instruction.h"
using namespace std;

CreateDataBaseInst::CreateDataBaseInst(string s){
    static Parser ps;
    ps.reset(s);
    ps.match_token("create");
    ps.match_token("database");
    dbname=ps.get_str();
}

void CreateDataBaseInst::exec(SQL &sql){
    sql.create_database(dbname);
}