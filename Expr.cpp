#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include "Expr.h"
using namespace std;

Parser Expr::ps;
Parser RecordExpr::ps;
Parser GroupExpr::ps;

Expr::Expr(const string& s, const map<string, int> &n){
    for(auto name: n) name2id[name.first] = name.second;
    raw = s;
    //将表达式的字符串处理为node序列
    queue<shared_ptr<Node>> nodes;
    ps.resetExpr(s);
    while(!ps.ended()){
        string token = ps.get_str();
        if(token == "("){
            nodes.push(make_shared<BracketNode>(true));
        }else if(token == ")"){
            nodes.push(make_shared<BracketNode>(false));
        }else if(Operator::hasOptr(utils::tolower(token))){
            nodes.push(make_shared<Optr>(utils::tolower(token)));
        }else if(RecordFunc::hasFunc(utils::tolower(token)) || GroupFunc::hasFunc(utils::tolower(token))){
            nodes.push(make_shared<FuncNode>(utils::tolower(token)));
        }else if(token == ","){
            nodes.push(make_shared<BracketNode>(false));
            nodes.push(make_shared<BracketNode>(true));
        }else{
            nodes.push(OpndNode::create_opndnode(token, n));
        }
    }
    //建立逆波兰表达式的vector
    stack<shared_ptr<OptrNode>> optrs;
    while(!nodes.empty()){
        shared_ptr<Node> node= nodes.front();
        nodes.pop();
        if(node->isOptr()){
            auto optrnode = dynamic_pointer_cast<OptrNode>(node);
            if(optrnode->isBracket()){
                auto bracketnode = dynamic_pointer_cast<BracketNode>(optrnode);
                if(bracketnode->left){
                    optrs.push(bracketnode);
                }else{
                    while(!optrs.empty()){
                        auto optrnode2 = optrs.top();
                        if(optrnode2->isBracket()){
                            optrs.pop();
                            break;
                        }else{
                            poland.push_back(optrnode2);
                            optrs.pop();
                        }
                    }
                }
            }else if(optrnode->isFunc()){
                optrs.push(optrnode);
            }else{
                while(!optrs.empty() && !optrs.top()->isBracket() && (optrs.top()->isFunc() || (optrs.top()->get_priority() >= optrnode->get_priority())) ){
                    poland.push_back(optrs.top());
                    optrs.pop();
                }
                optrs.push(optrnode);
            }
        }else{
            poland.push_back(node);
        }
    }
    while(!optrs.empty()){
        poland.push_back(optrs.top());
        optrs.pop();
    }
}

shared_ptr<Value> RecordExpr::get_value(const Record& record){
    stack<shared_ptr<Value>> values;
    for(auto node : poland){
        if(node->isOptr()){
            auto optrnode = dynamic_pointer_cast<OptrNode>(node);
            if(optrnode->isFunc()){
                auto func=dynamic_pointer_cast<FuncNode>(optrnode);
                vector<shared_ptr<Value>> params;
                int num=func->get_paramnum();
                while(num--){
                    params.push_back(values.top());
                    values.pop();
                }
                reverse(params.begin(), params.end());
                values.push(func->caculate(params));
            }else{
                auto optr = dynamic_pointer_cast<Optr>(optrnode);
                if(optr->isBinary()){
                    auto v2 = values.top();
                    values.pop();
                    auto v1 = values.top();
                    values.pop();
                    values.push(optr->caculate(v1, v2));
                }else{
                    auto v = values.top();
                    values.pop();
                    values.push(optr->caculate(v));
                }
            }
        }else{
            values.push(dynamic_pointer_cast<OpndNode>(node)->get_value(record));
        }
    }
    return values.top();
}

shared_ptr<Value> GroupExpr::get_value(shared_ptr<Group>& group){
    return group->caculatePoland(poland);
}

bool GroupExpr::isGroupExpr(string& s){
    ps.reset(s);
    while(!ps.ended()){
        if(GroupFunc::hasFunc(utils::tolower(ps.get_str()))) return true;
    }
    return false;
}
