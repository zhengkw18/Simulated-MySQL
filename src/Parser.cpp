#include <string>
#include <sstream>
#include <cassert>
#include <utility>
#include <iostream>
#include "Parser.h"
#include "utils.h"
using namespace utils;
using namespace std;
#define pb push_back

bool Parser::is_oper(char c){
    static const char str[]="=<>|&!";
    for(const char *s=str;*s;++s){
        if(*s==c) return true;
    }
    return false;
}

bool Parser::is_singleoper(char c){
    static const char str[]="(),+-*/%";
    for(const char *s=str;*s;++s){
        if(*s==c) return true;
    }
    return false;
}

bool Parser::is_oper_(char c){
    static const char str[]="()*,=<>";
    for(const char *s=str;*s;++s){
        if(*s==c) return true;
    }
    return false;
}
//将字符串格式化，符号以空格隔开，如and a!=2 变为and a != 2
void Parser::reset(const string &s){
    string tmp;
    bool in_quotation=false;
    for(char c:s){
		if (c == '\"' || c == '\'') {
			in_quotation ^= 1;
		}
        if(!in_quotation&& is_oper_(c)){
            tmp.pb(' ');
            tmp.pb(c);
            tmp.pb(' ');
        }else{
            tmp.pb(c);
        }
    }
    text=tmp;
    utils::string_replace(text, "\n", "");
    is_end=false;
    pos=0;
    skip();
}

void Parser::resetExpr(const string &s){
    string tmp;
    bool in_quotation=false,in_oper=false;
    for(char c:s){
		if (c == '\"' || c == '\'') {
            if(!in_quotation)tmp.pb(' ');
            tmp.pb(c);
			in_quotation ^= 1;
			in_oper = false;
            continue;
		}
        if(!in_quotation){
            if(is_singleoper(c)){
                in_oper=false;
                if(c == '-' && (utils::getlast_notblank(tmp) == 0|| utils::getlast_notblank(tmp) == '(' || utils::getlast_notblank(tmp) == ',')){//特殊处理可能是负号的减号：左侧为空,左括号，逗号
                    tmp.pb('0');
                }
                tmp.pb(' ');
                tmp.pb(c);
                tmp.pb(' ');
                continue;
            }
            if(in_oper){
                if(is_oper(c)){
                    tmp.pb(c);
                }else{
                    tmp.pb(' ');
                    tmp.pb(c);
                    in_oper=false;
                }
            }else{
                if(is_oper(c)){
                    tmp.pb(' ');
                    tmp.pb(c);
                    in_oper=true;
                }else{
                    tmp.pb(c);
                }
            }
        }else{
            tmp.pb(c);
        }
    }
    text=tmp;
    utils::string_replace(text, "\n", "");
    is_end=false;
    pos=0;
    skip();
}

void Parser::skip(int cnt){
    head="";
    for(;cnt>0;--cnt){
        if(is_end) return;
        while(text[pos] == ' ')pos++;
        while(pos != text.length() && text[pos] != ' '){
            if(text[pos]=='\"' || text[pos]=='\''){
                head+=text[pos];
                pos++;
                while(text[pos]!='\"' && text[pos]!='\''){
                    head+=text[pos];
                    pos++;
                }
                head+=text[pos];
                pos++;
            }else{
                head+=text[pos];
                pos++;
            }
        }
    }
    if(head=="")is_end=true;
}

bool Parser::ended(){
    return is_end;
}
//返回头
string Parser::lookahead(){
    return head;
}
//检查头是否匹配传入字符串
bool Parser::lookahead(const string &s){
    return tolower(s)==tolower(head);
}
//必须成功匹配传入字符串，将指向后移
void Parser::match_token(const string &s){
    assert(tolower(s)==tolower(head));
    skip();
}
//得到头并将指向后移
string Parser::get_str(){
    if(is_end) return "#";
    string tmp=move(head);
    skip();
    return tmp;
}
