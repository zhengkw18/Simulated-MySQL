#pragma once
#include "Record.h"
#include "Attribute.h"
#include "Value.h"
#include "WhereClauses.h"
#include <set>
#include <map>
#include <memory>
#include <vector>
#include <cassert>
#include <iostream>
#include "Group.h"
using namespace std;

class vTable{
    vector<Attribute> id2attr;
    map<string, int> name2id;

    vector<Record> records;
public:
    static shared_ptr<vTable> getEmpty();//得到一张空表
    //求两张表的笛卡尔积，s1,s2分别为t1,t2的表名，在合并时字段名变为XX.XX，留空代表不继续添加.
    static shared_ptr<vTable> getCartesian(shared_ptr<vTable> t1, shared_ptr<vTable> t2, string s1, string s2);
    vTable(const vector<Attribute>&);
    void insert_into(Record&);
    Record& getRecord(int id);
    int addColumn(string&);//添加某RecordExpr确定的字段，返回对应id
    void addColumn(string&, vector<shared_ptr<Value>>&);
    vector<Attribute>& export_id2attr();
    vector<string> export_id2name();
    map<string, int>& export_name2id();
    void applywhere(const WhereClauses&);
    void output(vector<string>& selected, map<string, string>& aliases, ostream& o, bool);
    void outputall(ostream& o, bool);
    vector<shared_ptr<Group>> getGrouped(string& groupby);
    void order(vector<string>& orders);
    shared_ptr<Group> getWholeAsGroup();
    //和order联合使用实现union (all)功能
    static shared_ptr<vTable> unionvtable(shared_ptr<vTable> t1, shared_ptr<vTable> t2, vector<string> se1, vector<string> se2);
    void deleteRepeated();
    //join
    //type:
    //inner:0
    //left:1
    //right:2
    static shared_ptr<vTable> jointable(shared_ptr<vTable> t1, shared_ptr<vTable> t2, string s1, string s2, string& onclausestr,int type);
};

class RecordMultipleComparator{
    vector<int> orders;
public:
    RecordMultipleComparator(vector<int>&);
    bool operator()(const Record&, const Record&);
};
