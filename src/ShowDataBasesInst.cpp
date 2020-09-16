#include "Instruction.h"
#include "Parser.h"
using namespace std;

ShowDataBasesInst::ShowDataBasesInst(string s){
    static Parser ps;
    ps.reset(s);
    ps.match_token("show");
    ps.match_token("databases");
}

void ShowDataBasesInst::exec(SQL &sql){
    sql.show_databases();
}