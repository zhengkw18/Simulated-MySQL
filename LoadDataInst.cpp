#include "Instruction.h"
#include <fstream>
Parser LoadDataInst::ps;

LoadDataInst::LoadDataInst(string s) {
    ps.reset(s);

    ps.match_token("load");
    ps.match_token("data");
    if(ps.lookahead("local")) ps.skip();
    ps.match_token("infile");

    filename=ps.get_str();
    filename=filename.substr(1,filename.length()-2);

    ps.match_token("into");
    ps.match_token("table");

    tablename = ps.get_str();
    if(ps.lookahead("(")) {
        ps.skip();
        while(!ps.ended() && ps.lookahead(",")){
            ps.skip();
            fieldnames.push_back(ps.get_str());
        }
    }
}

void LoadDataInst::exec(SQL &sql){
    shared_ptr<Table> table = sql.get_current_database()->get_table(tablename);
    ifstream fin(filename);
    if(fin){
        string s;
        while(getline(fin, s)){
            stringstream ss(s);
            vector<string> values;
            string value;
            while(ss>>value)values.push_back(value);
            if(fieldnames.size()==0){
                vector<Attribute>& attibutes = table->export_id2attr();
                for(auto& attribute : attibutes)fieldnames.push_back(attribute.get_name());
            }
            for(int i=0;i<fieldnames.size();i++){
                //如果是字符串/日期/时间，在两端加上引号
                string type=table->export_id2attr()[table->export_name2id()[fieldnames[i]]].get_type();
                if(type=="char(1)" || type=="date" || type=="time")values[i]="\""+values[i]+"\"";
            }
            if(fieldnames.size()==values.size()){
                table->insert_into(fieldnames, values);
            }
        }
    }
    fin.close();
}
