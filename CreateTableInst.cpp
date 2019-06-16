#include "Instruction.h"
#include "utils.h"
#include<utility>
#include<memory>
using namespace std;
using namespace utils;

Parser CreateTableInst::ps;

void CreateTableInst::parseAttribute(){
    string name=ps.get_str();
    string type=ps.get_str();
    if(tolower(type)=="key"){ //primary key
        ps.match_token("(");
        primary=ps.get_str();
        ps.match_token(")");
    }else{ //Attribute
        if(ps.lookahead("not")){
            ps.match_token("not");
            ps.match_token("null");
            attrs.push_back(Attribute(name,type,attrs.size(),true));
        }else{
            attrs.push_back(Attribute(name,type,attrs.size(),false));
        }
    }
}

void CreateTableInst::parseAttrs(){
    while(true){
        parseAttribute();
        if(ps.lookahead(")")) return;
        ps.match_token(",");
    }
}


CreateTableInst::CreateTableInst(string s){
    ps.reset(s);
    ps.match_token("create");
    ps.match_token("table");
    tableName=ps.get_str();
    ps.match_token("(");
    parseAttrs();
    ps.match_token(")");
}

void CreateTableInst::exec(SQL &sql){
    shared_ptr<DataBase> db=sql.get_current_database();
    db->create_table(tableName,attrs,primary);
}
