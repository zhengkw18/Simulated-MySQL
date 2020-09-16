#include "SyntaxTree.h"
#include "Record.h"
#include "Value.h"
#include <cassert>
#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

using namespace std;
bool is_integer(const string &number) {
    return number.find('.') == string::npos;
}

std::shared_ptr<OpndNode> OpndNode::create_opndnode(const string &s, const map<string, int> &name2id) {
    if (name2id.find(s) != name2id.end()) {	//attr
        return make_shared<AttrNode>(name2id.at(s));
    } else {		//value
        ptr_v value;
        if (s[0] == '\'' || s[0] == '\"') {
            value =
                make_shared<CharValue>(s.substr(1, s.length() - 2));
        }
        else if (is_integer(s)) {
            value = make_shared<IntValue>(std::stoi(s));
        } else {
            value = make_shared<DoubleValue>(std::stod(s));
        }
        return make_shared<ConstNode>(value);
    }
}

ptr_v FuncNode::caculate(vector<shared_ptr<Value>>& params) const{
    if(RecordFunc::hasFunc(funcname)){
        return RecordFunc::name_expr[funcname](params);
    }else if(GroupFunc::hasFunc(funcname)){
        return GroupFunc::name_expr[funcname](params);
    }
    return nullptr;

}
//v1相较于v2先入栈
ptr_v Optr::caculate(ptr_v v1, ptr_v v2) const{
    if(isBinary()){
        return Operator::optr_binary[op_name](v1,v2);
    }else{
        return Operator::optr_unit[op_name](v1);
    }
}
