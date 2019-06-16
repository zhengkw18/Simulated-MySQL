#pragma once
#include "Record.h"
#include "Value.h"
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "Operator.h"
#include "Func.h"

using namespace std;
class Node {
public:
    virtual bool isOptr() const = 0;
};
//²Ù×÷Êý
class OpndNode : public Node{
public:
    virtual bool isOptr() const {return false;}
    static shared_ptr<OpndNode> create_opndnode(const string &, const map<string, int> &);
    virtual ptr_v get_value(const Record &) const = 0;
};
class ConstNode : public OpndNode {
    ptr_v value;
  public:
    ConstNode(ptr_v value) : value(value) {}
    ptr_v get_value(const Record &) const { return value; }
};

class AttrNode : public OpndNode {
    int id;
  public:
    AttrNode(int id): id(id){}
    ptr_v get_value(const Record & rec) const { return rec.get_field(id); }
};

//²Ù×÷·û
class OptrNode : public Node {
  public:
    virtual bool isOptr() const {return true;}
    virtual bool isBracket() const = 0;
    virtual bool isFunc() const = 0;
    virtual int get_priority() const = 0;
};

class Optr : public OptrNode{
    string op_name;
  public:
    bool isBracket() const {return false;}
    bool isFunc() const {return false;}
    int get_priority() const {return Operator::priority[op_name];}
    Optr(string op_name):op_name(op_name){}
    bool isBinary() const {return Operator::isBinary(op_name);}
    ptr_v caculate(ptr_v v1, ptr_v v2 = nullptr) const;
};

class FuncNode : public OptrNode {
    string funcname;
public:
    bool isBracket() const {return false;}
    bool isFunc() const {return true;}
    bool is_countall() const {return funcname == "countall";}
    int get_priority() const {return 10;}
    bool isGroupFunc() const {return GroupFunc::hasFunc(funcname);}
    int get_paramnum() const {return RecordFunc::param_num[funcname];}
    FuncNode(string funcname): funcname(funcname){}
    ptr_v caculate(vector<shared_ptr<Value>>& params) const;
};

class BracketNode : public OptrNode {
  public:
    bool isBracket() const {return true;}
    bool isFunc() const {return false;}
    int get_priority() const {return 0;}
    bool left;
    BracketNode(bool left): left(left){}
};
