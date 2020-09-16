#include "Instruction.h"
#include <cstring>
using namespace std;

Parser InsertIntoInst::ps;

InsertIntoInst::InsertIntoInst(string s) {
	ps.reset(s);

	ps.match_token("insert");
	ps.match_token("into");

	tablename = ps.get_str();

	ps.match_token("(");
	attrnames.push_back(move(ps.get_str()));
	while (true) {
		if (ps.lookahead(")")) break;
		ps.match_token(",");
		attrnames.push_back(move(ps.get_str()));
	}
	ps.match_token(")");

	ps.match_token("values");

	ps.match_token("(");

	string tmp;
    while(true) {
        if(ps.lookahead(")")) break;
		tmp = ps.get_str();
        values.push_back(tmp);
        if (ps.lookahead(",")) ps.skip();
    }
	ps.skip();
    assert(attrnames.size() == values.size());
}

void InsertIntoInst::exec(SQL &sql){
    shared_ptr<Table> table = sql.get_current_database()->get_table(tablename);
    table->insert_into(attrnames, values);
}
