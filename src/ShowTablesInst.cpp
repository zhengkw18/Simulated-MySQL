#include "Instruction.h"
#include "Parser.h"
using namespace std;

ShowTablesInst::ShowTablesInst(string s){
    static Parser ps;
    ps.reset(s);
    ps.match_token("show");
    ps.match_token("tables");
}

void ShowTablesInst::exec(SQL &sql){
    sql.get_current_database()->show_tables();
}