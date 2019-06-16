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
    //������Ӿ�
    //�Ӿ�ؼ���λ��-�Ӿ���ŵ�pair
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
    //���ܵ�count(*)�������滻Ϊcountall���⴦��
    for(int i=0;i<6;i++){
        utils::trim(clauses[i]);
        utils::string_replace(clauses[i], "(*)", "all");
    }
    //����select�Ӿ䣬����as
    if(clauses[0] == "*")selectall = true;
    else{
        vector<int> positions;//��¼���в��������ڵĶ��ŵ�λ��
        positions.push_back(-1);//ͷ�����ⶺ��
        int layer=0;//���Ų���
        for(int i=0;i<clauses[0].length();i++){
            if(clauses[0][i] == ',' && layer == 0)positions.push_back(i);
            if(clauses[0][i] == '(')layer++;
            if(clauses[0][i] == ')')layer--;
        }
        positions.push_back(clauses[0].length());//β�����ⶺ��
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
    //����from table1, table2����
    if(haskeyword[1]) utils::split(clauses[1], tablenames, ",");
    //����ordered�����Ӿ�
    if(haskeyword[3]){
        vector<int> positions;//��¼���в��������ڵĶ��ŵ�λ��
        positions.push_back(-1);//ͷ�����ⶺ��
        int layer=0;//���Ų���
        for(int i=0;i<clauses[3].length();i++){
            if(clauses[3][i] == ',' && layer == 0)positions.push_back(i);
            if(clauses[3][i] == '(')layer++;
            if(clauses[3][i] == ')')layer--;
        }
        positions.push_back(clauses[3].length());//β�����ⶺ��
        for(int i=0;i<positions.size()-1;i++){
            string s = clauses[3].substr(positions[i]+1, positions[i+1]-positions[i]-1);
            utils::trim(s);
            orders.push_back(s);
        }
    }
/*�������
��ʱ
select: selected, aliases
from: tablenames
group by(���Record�ı��ʽ): clauses[2]
order by(���Record(������)��Group(����)�ı��ʽ): orders

where(���Record�ı��ʽ): clauses[4]
filename(������): clauses[5]
*/
}

void SelectInst::construct_vtable(SQL& sql){
    if(haskeyword[1]){
        //����from
        vtable = sql.get_current_database()->get_table(tablenames[0])->get_vTable();
        if(tablenames.size()>1){
            vtable = vTable::getCartesian(vtable, sql.get_current_database()->get_table(tablenames[1])->get_vTable(), tablenames[0], tablenames[1]);
            for(int i=2; i<tablenames.size(); i++){
                vtable= vTable::getCartesian(vtable, sql.get_current_database()->get_table(tablenames[i])->get_vTable(), "", tablenames[i]);
            }
        }
        //����where
        if(haskeyword[4]){
            WhereClauses whereclause(clauses[4], vtable->export_name2id());
            vtable->applywhere(whereclause);
        }
        //������group�������
        if(haskeyword[2]){
            //�ȷ���
            vector<shared_ptr<Group>> groups = vtable->getGrouped(clauses[2]);
            //����selected��orders�е�����GroupExpr
            set<string> fieldnames; //��¼���п��ܳ��ֵ��ֶ�����ʹ��setȥ��
            fieldnames.insert(clauses[2]);
            for(string order : orders)fieldnames.insert(order);
            if(!selectall){
                for(string s : selected)fieldnames.insert(s);
            }
            //����ֵ���Թ����µ�vtable
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
            //��Groups�����µ�vtable
            vtable = make_shared<vTable>(attrs);
            for(Record r : records)vtable->insert_into(r);
            //��orders����
            if(haskeyword[3])vtable->order(orders);
        }else{
            //����order(RecordExpr)
            if(haskeyword[3])vtable->order(orders);
            //����select(Ĭ��ֻ��GroupExpr,����ֻ��RecordExpr,��������δ���������û��ָ�������ʽ)
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
        //û��from��ֻ�账��selected
        vtable = vTable::getEmpty();
        for(string s : selected){
            vtable->addColumn(s);
        }
    }
}
void SelectInst::exec(SQL& sql){
    //����vtable
    construct_vtable(sql);
    //output
    if(haskeyword[5]){
        //������ļ�
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
        //��׼���
        if(selectall){
            vtable->outputall(cout, true);
        }else{
            vtable->output(selected, aliases, cout, true);
        }
    }
}
