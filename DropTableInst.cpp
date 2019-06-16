#include "Instruction.h"
#include <sstream>
using namespace std;

DropTableInst::DropTableInst(string s){
    static Parser ps;
    ps.reset(s);
    ps.match_token("drop");
    ps.match_token("table");
    tableName=ps.get_str();
}

void DropTableInst::exec(SQL &sql){
    sql.get_current_database()->drop_table(tableName);
}