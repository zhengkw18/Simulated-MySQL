#include "Instruction.h"
using namespace std;

ShowColumnsFromInst::ShowColumnsFromInst(string s){
    static Parser ps;
    ps.reset(s);
    ps.match_token("show");
    ps.match_token("columns");
    ps.match_token("from");
    tableName=ps.get_str();
}

void ShowColumnsFromInst::exec(SQL &sql){
    sql.get_current_database()->get_table(tableName)->show_columns();
}