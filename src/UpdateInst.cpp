#include "Instruction.h"
using namespace std;

Parser UpdateInst::ps;

UpdateInst::UpdateInst(string s){
    ps.reset(s);

    ps.match_token("update");
    tablename = ps.get_str();
    ps.match_token("set");

    attrname = ps.get_str();
    ps.match_token("=");
    attrexp = ps.get_str();

    if(ps.lookahead("where")) {
        ps.skip();
        whereclauses_str = ps.get_str();
        while(!ps.ended() && !ps.lookahead(";")) whereclauses_str = whereclauses_str + " " + ps.get_str();
    }
}

void UpdateInst::exec(SQL &sql){
    shared_ptr<Table> table = sql.get_current_database()->get_table(tablename);
    table->update(attrname, attrexp, WhereClauses(whereclauses_str, table->export_name2id()));
}
