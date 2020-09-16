#include "Instruction.h"
using namespace std;

DropDataBaseInst::DropDataBaseInst(string s){
    static Parser ps;
    ps.reset(s);
    ps.match_token("drop");
    ps.match_token("database");
    dbname=ps.get_str();
}

void DropDataBaseInst::exec(SQL &sql){
    sql.drop_database(dbname);
}