#pragma once
#include<string>
#include<sstream>

class Parser{
    std::string text;
    std::string head;
    bool is_end;
    int pos = 0;
public:
    Parser()=default;
    static bool is_oper(char);
    static bool is_oper_(char);
    static bool is_singleoper(char);
    void reset(const std::string &);
    void resetExpr(const std::string &);
    std::string lookahead();
    bool lookahead(const std::string&);
    bool ended();
    void skip(int cnt=1);
    void match_token(const std::string &);
    std::string get_str();
};
