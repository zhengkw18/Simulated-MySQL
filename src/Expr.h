#pragma once
#include "Parser.h"
#include "SyntaxTree.h"
#include "utils.h"
#include "Value.h"
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include"Group.h"
using namespace std;
class Expr{
    static Parser ps;
    //处理得到结点的逆波兰表达式
    map<string, int> name2id;
    string raw;
public:
    vector<shared_ptr<Node>> poland;
    //用关于记录的表达式语句来构造
    Expr(const string& s, const map<string, int> &n);
};

class RecordExpr: public Expr{
    static Parser ps;
public:
    RecordExpr(const string& s, const map<string, int> &n) : Expr(s,n){}
    shared_ptr<Value> get_value(const Record& record);
};

class GroupExpr : public Expr{
    static Parser ps;
public:
    static bool isGroupExpr(string& s);
    GroupExpr(const string& s, const map<string, int> &n) : Expr(s,n){}
    shared_ptr<Value> get_value(shared_ptr<Group>& group);
};
