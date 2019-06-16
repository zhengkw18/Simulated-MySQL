#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstring>
#include <set>
#include <assert.h>
#include "utils.h"
#include "Group.h"
#include "Instruction.h"
#include "Expr.h"
using namespace std;

Parser SelectInst::ps;

SelectInst::SelectInst(string s){
    utils::trim(s);
    string lows = utils::tolower(s);
    if(lows.find("countall") != string::npos)assert(false);
    //分理出子句
    //子句关键字位置-子句序号的pair
    vector<pair<int, int>> indexs;
    map<int, string> keywords={
    {0, "select "},
    {1, " from "},
    {2, " group by "},
    {3, " order by "},
    {4, " where "},
    {5, " into "}
    };
    for(int i=0;i<6;i++){
        int pos;
        if((pos = lows.find(keywords[i])) != string::npos){
            haskeyword[i]=true;
            indexs.push_back(make_pair(pos, i));
        }
    }
    sort(indexs.begin(), indexs.end());
    for(int i=0;i<indexs.size()-1;i++){
        int index = indexs[i].second;
        int pos = indexs[i].first;
        int nextpos = indexs[i+1].first;
        clauses[index] = s.substr(pos+keywords[index].length(), nextpos-pos-keywords[index].length());
    }
    int lastindex = indexs[indexs.size()-1].second;
    int lastpos = indexs[indexs.size()-1].first;
    clauses[lastindex] = s.substr(lastpos+keywords[lastindex].length(), indexs.size()-lastpos-keywords[lastindex].length());
    //可能的count(*)，将其替换为countall特殊处理
    for(int i=0;i<6;i++){
        utils::trim(clauses[i]);
        utils::string_replace(clauses[i], "(*)", "all");
    }
    //处理select子句，分离as
    if(clauses[0] == "*")selectall = true;
    else{
        vector<int> positions;//记录所有不在括号内的逗号的位置
        positions.push_back(-1);//头部虚拟逗号
        int layer=0;//括号层数
        for(int i=0;i<clauses[0].length();i++){
            if(clauses[0][i] == ',' && layer == 0)positions.push_back(i);
            if(clauses[0][i] == '(')layer++;
            if(clauses[0][i] == ')')layer--;
        }
        positions.push_back(clauses[0].length());//尾部虚拟逗号
        for(int i=0;i<positions.size()-1;i++){
            string s = clauses[0].substr(positions[i]+1, positions[i+1]-positions[i]-1);
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
    //处理from table1, table2……
    if(haskeyword[1]) utils::split(clauses[1], tablenames, ",");
    //处理ordered划分子句
    if(haskeyword[3]){
        vector<int> positions;//记录所有不在括号内的逗号的位置
        positions.push_back(-1);//头部虚拟逗号
        int layer=0;//括号层数
        for(int i=0;i<clauses[3].length();i++){
            if(clauses[3][i] == ',' && layer == 0)positions.push_back(i);
            if(clauses[3][i] == '(')layer++;
            if(clauses[3][i] == ')')layer--;
        }
        positions.push_back(clauses[3].length());//尾部虚拟逗号
        for(int i=0;i<positions.size()-1;i++){
            string s = clauses[3].substr(positions[i]+1, positions[i+1]-positions[i]-1);
            utils::trim(s);
            orders.push_back(s);
        }
    }
/*处理完毕
此时
select: selected, aliases
from: tablenames
group by(针对Record的表达式): clauses[2]
order by(针对Record(不分组)或Group(分组)的表达式): orders

where(针对Record的表达式): clauses[4]
filename(带引号): clauses[5]
*/
}

void SelectInst::construct_vtable(SQL& sql){
    if(haskeyword[1]){
        //处理from
        vtable = sql.get_current_database()->get_table(tablenames[0])->get_vTable();
        if(tablenames.size()>1){
            vtable = vTable::getCartesian(vtable, sql.get_current_database()->get_table(tablenames[1])->get_vTable(), tablenames[0], tablenames[1]);
            for(int i=2; i<tablenames.size(); i++){
                vtable= vTable::getCartesian(vtable, sql.get_current_database()->get_table(tablenames[i])->get_vTable(), "", tablenames[i]);
            }
        }
        //处理where
        if(haskeyword[4]){
            WhereClauses whereclause(clauses[4], vtable->export_name2id());
            vtable->applywhere(whereclause);
        }
        //分有无group两种情况
        if(haskeyword[2]){
            //先分组
            vector<shared_ptr<Group>> groups = vtable->getGrouped(clauses[2]);
            //调出selected和orders中的所有GroupExpr
            set<string> fieldnames; //记录所有可能出现的字段名，使用set去重
            fieldnames.insert(clauses[2]);
            for(string order : orders)fieldnames.insert(order);
            if(!selectall){
                for(string s : selected)fieldnames.insert(s);
            }
            //计算值用以构建新的vtable
            vector<Attribute> attrs;
            vector<Record> records;
            for(string f: fieldnames)attrs.push_back(Attribute(f, "int", 0, false));
            for(shared_ptr<Group> group : groups){
                vector<shared_ptr<Value>> values;
                values.clear();
                for(string field : fieldnames){
                    GroupExpr groupexpr(field, vtable->export_name2id());
                    values.push_back(groupexpr.get_value(group));
                }
                Record record(values);
                records.push_back(record);
            }
            //由Groups构建新的vtable
            vtable = make_shared<vTable>(attrs);
            for(Record r : records)vtable->insert_into(r);
            //由orders排序
            if(haskeyword[3])vtable->order(orders);
        }else{
            //处理order(RecordExpr)
            if(haskeyword[3])vtable->order(orders);
            //处理select(默认只有GroupExpr,或者只有RecordExpr,否则属于未定义情况，没有指定输出格式)
            if(!selectall){
                if(GroupExpr::isGroupExpr(selected[0])){
                    shared_ptr<Group> group = vtable->getWholeAsGroup();
                    vector<Attribute> attrs;
                    for(string s : selected){
                        Attribute attr(s, "int", 0, false);
                        attrs.push_back(attr);
                    }
                    shared_ptr<vTable> newvtable = make_shared<vTable>(attrs);
                    vector<shared_ptr<Value>> values;
                    for(string s : selected){
                        GroupExpr groupexpr(s, vtable->export_name2id());
                        values.push_back(groupexpr.get_value(group));
                    }
                    Record re(values);
                    vtable = make_shared<vTable>(attrs);
                    vtable->insert_into(re);
                }else{
                    for(string s : selected) vtable->addColumn(s);
                }
            }
        }
    }else{
        //没有from，只需处理selected
        vtable = vTable::getEmpty();
        for(string s : selected){
            vtable->addColumn(s);
        }
    }
}
void SelectInst::exec(SQL& sql){
    //构建vtable
    construct_vtable(sql);
    //output
    if(haskeyword[5]){
        //输出到文件
        string filename = clauses[5].substr(9, clauses[5].length()-10);
        ifstream fin(filename);
        if(fin){
            fin.close();
        }else{
            ofstream fout(filename);
            if(selectall){
                vtable->outputall(fout, false);
            }else{
                vtable->output(selected, aliases, fout, false);
            }
            fout.close();
        }
    }else{
        //标准输出
        if(selectall){
            vtable->outputall(cout, true);
        }else{
            vtable->output(selected, aliases, cout, true);
        }
    }
}
