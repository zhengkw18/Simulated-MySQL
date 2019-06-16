#include "Instruction.h"
using namespace std;

UseInst::UseInst(string s){
    static Parser ps;
    ps.reset(s);
    ps.match_token("use");
    dbname=ps.get_str();
}

void UseInst::exec(SQL &sql){
    sql.use(dbname);
}