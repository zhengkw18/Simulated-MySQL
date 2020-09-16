#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstring>
#include <assert.h>
#include "utils.h"
#include "Group.h"
#include "Instruction.h"
#include "Expr.h"
using namespace std;
SelectJoinInst::SelectJoinInst(string info){
    utils::trim(info);
    string lowinfo=utils::tolower(info);
    int frompos = lowinfo.find(" from ");
    int onpos = lowinfo.find(" on ");
    //处理两个table和别名
    int joinpos;
    string s1,s2;
    if((joinpos = lowinfo.find(" inner join ")) != string::npos){
        type=0;
        s1=info.substr(frompos+6, joinpos-frompos-6);
        s2=info.substr(joinpos+12, onpos-joinpos-12);
    }else if((joinpos = lowinfo.find(" left join ")) != string::npos){
        type=1;
        s1=info.substr(frompos+6, joinpos-frompos-6);
        s2=info.substr(joinpos+11, onpos-joinpos-11);
    }else if((joinpos = lowinfo.find(" right join ")) != string::npos){
        type=2;
        s1=info.substr(frompos+6, joinpos-frompos-6);
        s2=info.substr(joinpos+12, onpos-joinpos-12);
    }else{assert(false);}
    stringstream ss1(s1);
    ss1>>table1;
    if(!(ss1>>aliase1))aliase1=table1;
    stringstream ss2(s2);
    ss2>>table2;
    if(!(ss2>>aliase2))aliase2=table2;
    utils::trim(table1);
    utils::trim(table2);
    utils::trim(aliase1);
    utils::trim(aliase2);
    //处理selectclause
    selectclause = info.substr(7, frompos-7);
    utils::trim(selectclause);
    if(selectclause == "*")selectall = true;
    else{
        vector<int> positions;//记录所有不在括号内的逗号的位置
        positions.push_back(-1);//头部虚拟逗号
        int layer=0;//括号层数
        for(int i=0;i<selectclause.length();i++){
            if(selectclause[i] == ',' && layer == 0)positions.push_back(i);
            if(selectclause[i] == '(')layer++;
            if(selectclause[i] == ')')layer--;
        }
        positions.push_back(selectclause.length());//尾部虚拟逗号
        for(int i=0;i<positions.size()-1;i++){
            string s = selectclause.substr(positions[i]+1, positions[i+1]-positions[i]-1);
            utils::trim(s);
            selected.push_back(s);
        }
    }
    for(int i=0;i<selected.size();i++){
        string s = selected[i];
        string low = utils::tolower(s);
        int pos;
        if((pos = low.find(" as ")) != string::npos){
            selected[i] = s.substr(0, pos);
            aliases[selected[i]] = s.substr(pos+4,s.length()-pos-4);
        }else{
            aliases[s] = s;
        }
    }
    onclausestr=info.substr(onpos+4, info.length()-onpos-4);
    utils::trim(onclausestr);
}

void SelectJoinInst::exec(SQL& sql){
    //构建vtable
    shared_ptr<vTable> vtable1 = sql.get_current_database()->get_table(table1)->get_vTable();
    shared_ptr<vTable> vtable2 = sql.get_current_database()->get_table(table2)->get_vTable();
    vtable = vTable::jointable(vtable1, vtable2, aliase1, aliase2, onclausestr, type);
    //output
    if(selectall){
        vtable->outputall(cout, true);
    }else{
        vtable->output(selected, aliases, cout, true);
    }
}
