#include "Instruction.h"
using namespace std;

Parser DeleteFromInst::ps;

DeleteFromInst::DeleteFromInst(string s) {
    ps.reset(s);

    ps.match_token("delete");
    ps.match_token("from");

    tablename = ps.get_str();
    if(ps.lookahead("where")) {
        ps.skip();
        whereclauses_str = ps.get_str();
        while(!ps.ended() && !ps.lookahead(";")) whereclauses_str = whereclauses_str + " " + ps.get_str();
    }
}

void DeleteFromInst::exec(SQL &sql){
    shared_ptr<Table> table = sql.get_current_database()->get_table(tablename);
    table->delete_from(WhereClauses(whereclauses_str, table->export_name2id()));
}
