#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include <memory>
using namespace std;
namespace Operator{
    static map<string, int> priority={
    {"||", 1},
    {"or", 1},
    {"xor", 1},
    {"&&", 2},
    {"and", 2},
    {"not", 3},
    {"!", 3},
    {"=", 4},
    {">", 4},
    {"<", 4},
    {"!=", 4},
    {"like", 4},
    {"+", 5},
    {"-", 5},
    {"*", 6},
    {"/", 6},
    {"div", 6},
    {"%", 6},
    {"mod", 6}
    };
    static map<string, function<shared_ptr<Value>(shared_ptr<Value> a)>>
    optr_unit={
    {"!", [](ptr_v a) -> ptr_v { return Value::v_not(a); }},
    {"not", [](ptr_v a) -> ptr_v { return Value::v_not(a); }}
    };
    static map<string, function<shared_ptr<Value>(shared_ptr<Value> a, shared_ptr<Value> b)>>
    optr_binary={
    {"||", [](ptr_v a, ptr_v b) -> ptr_v { return Value::v_or(a, b); }},
    {"or", [](ptr_v a, ptr_v b) -> ptr_v { return Value::v_or(a, b); }},
	{"xor", [](ptr_v a, ptr_v b) -> ptr_v { return Value::v_xor(a, b); }},
	{"&&", [](ptr_v a, ptr_v b) -> ptr_v { return Value::v_and(a, b); }},
	{"and", [](ptr_v a, ptr_v b) -> ptr_v { return Value::v_and(a, b); }},
	{"<", [](ptr_v a, ptr_v b) -> ptr_v { return Value::v_lt(a, b); }},
	{">", [](ptr_v a, ptr_v b) -> ptr_v { return Value::v_gt(a, b); }},
	{"=", [](ptr_v a, ptr_v b) -> ptr_v { return Value::v_eq(a, b); }},
	{"!=",[](ptr_v a,ptr_v b)->ptr_v { return Value::v_not(Value::v_like(a,b)); }},
	{"like",[](ptr_v a,ptr_v b)->ptr_v { return Value::v_like(a,b); }},
	{"+",[](ptr_v a,ptr_v b)->ptr_v { return Value::v_add(a,b); }},
	{"-",[](ptr_v a,ptr_v b)->ptr_v { return Value::v_minus(a,b); }},
	{"*",[](ptr_v a,ptr_v b)->ptr_v { return Value::v_multiply(a,b); }},
	{"/",[](ptr_v a,ptr_v b)->ptr_v { return Value::v_div(a,b); }},
	{"div",[](ptr_v a,ptr_v b)->ptr_v { return Value::v_div(a,b); }},
	{"%",[](ptr_v a,ptr_v b)->ptr_v { return Value::v_mod(a,b); }},
	{"mod",[](ptr_v a,ptr_v b)->ptr_v { return Value::v_mod(a,b); }}
    };
    inline bool hasOptr(string s) {return priority.count(s);}
    inline bool isBinary(string s) {return optr_binary.count(s);}
}
