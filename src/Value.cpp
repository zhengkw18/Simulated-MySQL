#include "Value.h"
#include <cassert>
#include <iomanip>
#include <iostream>
#include "utils.h"
#include <memory>
#include <string>
#include <regex>

using namespace utils;
constexpr int months[2][13]={{0,31,28,31,30,31,30,31,31,30,31,30,31},{0,31,29,31,30,31,30,31,31,30,31,30,31}};

IntValue::IntValue(int value) : value(value) {}
DoubleValue::DoubleValue(double value) : value(value) {}
CharValue::CharValue(std::string value) : value(value) {}
void DateValue::setzero(){year=0;month=0;day=0;}
bool DateValue::isLeap(int y) const {return y%400==0 || (y%4==0 && y%100!=0);}
bool DateValue::isValid(){
    if(year<1000 || year>9999)return false;
    if(month<1 || month>12)return false;
    if(day<1 || day>months[isLeap(year)][month])return false;
    return true;
}
DateValue::DateValue(std::string value){
    int pos1,pos2;
    if((pos1 = value.find("-",0))!=string::npos){
        if((pos2 = value.find("-",pos1+1))!=string::npos){
            year=atoi(value.substr(0,pos1).c_str());
            month=atoi(value.substr(pos1+1,pos2-pos1).c_str());
            day=atoi(value.substr(pos2+1,value.length()-pos2).c_str());
        }
    }
    if(!isValid())setzero();
}
DateValue::DateValue(int v){
    if(v < 1 || v > 3287182){
        setzero();
    }else{
        int y = 1000;
        int tmp = 365 + isLeap(y);
        while(tmp < v){
            y++;
            tmp += 365 + isLeap(y);
        }
        year = y;
        int remnant = v - (tmp-365-isLeap(y));
        int m = 1;
        tmp = months[isLeap(year)][m];
        while(tmp < remnant){
            m++;
            tmp += months[isLeap(year)][m];
        }
        month = m;
        day = remnant - (tmp - months[isLeap(year)][month]);
    }
}
string DateValue::get_formated() const{
    string s;
    s+= std::to_string(year);
    s+= "-";
    if(month < 10)s+="0";
    s+= std::to_string(month);
    s+= "-";
    if(day < 10)s+="0";
    s+= std::to_string(day);
    return s;
}
int DateValue::get_val() const{
    int val = (year - 1000) * 365;
    val += (year - 1)/4 -249;
    val -= (year - 1)/100 -9;
    val += (year - 1)/400 -2;
    for(int i=1;i < month;i++)
        val += months[isLeap(year)][i];
    val += day;
    return val;
}
void TimeValue::setzero(){hour=0;minute=0;second=0;positive=true;}
bool TimeValue::isValid(){
    return (hour>=0 && hour<=838 && minute>=0 && minute<=59 && second>=0 &&second<=59);
}
TimeValue::TimeValue(std::string value){
    if(value[0]=='-'){
        positive=false;
        value=value.substr(1,value.length()-1);
    }
    int pos1,pos2;
    if((pos1 = value.find(":",0))!=string::npos){
        if((pos2 = value.find(":",pos1+1))!=string::npos){
            hour=atoi(value.substr(0,pos1).c_str());
            minute=atoi(value.substr(pos1+1,pos2-pos1).c_str());
            second=atoi(value.substr(pos2+1,value.length()-pos2).c_str());
        }
    }
    if(!isValid())setzero();
}
TimeValue::TimeValue(int v){
    if(v < 0){
        positive = false;
        v = -v;
    }
    if(v >= 3020400){
        setzero();
    }else{
        hour = v/3600;
        v %= 3600;
        minute = v/60;
        v %= 60;
        second = v;
    }
}
string TimeValue::get_formated() const{
    string s;
    if(!positive)s+="-";
    if(hour < 10)s+="0";
    s += std::to_string(hour);
    s += ":";
    if(minute < 10)s+="0";
    s += std::to_string(minute);
    s += ":";
    if(second < 10)s+="0";
    s += std::to_string(second);
    return s;
}
int TimeValue::get_val() const{
    int v = hour*3600 + minute*60 + second;
    if(!positive)v=-v;
    return v;
}
std::shared_ptr<DoubleValue> IntValue::to_double() const {
    return std::make_shared<DoubleValue>(value);
}

std::shared_ptr<DateValue> CharValue::to_date() const {
    return std::make_shared<DateValue>(value);
}

std::shared_ptr<TimeValue> CharValue::to_time() const {
    return std::make_shared<TimeValue>(value);
}

bool Value::operator<(const Value &) const { assert(false); return false; }

bool IntValue::operator<(const Value &v) const {
    return value < static_cast<const IntValue &>(v).value;
}

bool DoubleValue::operator<(const Value &v) const {
    return value < static_cast<const DoubleValue &>(v).value;
}

bool CharValue::operator<(const Value &v) const {
    return value < static_cast<const CharValue &>(v).value;
}

bool DateValue::operator<(const Value &v) const {
    const DateValue& dv= static_cast<const DateValue &>(v);
    if(year<dv.year)return true;
    if(year>dv.year)return false;
    if(month<dv.month)return true;
    if(month>dv.month)return false;
    if(day<dv.day)return true;
    return false;
}

bool TimeValue::operator<(const Value &v) const {
    const TimeValue& tv= static_cast<const TimeValue &>(v);
    int f1=2*positive-1;
    int f2=2*tv.positive-1;
    if(f1*hour<f2*tv.hour)return true;
    if(f1*hour>f2*tv.hour)return false;
    if(f1*minute<f2*tv.minute)return true;
    if(f1*minute>f2*tv.minute)return false;
    if(f1*second<f2*tv.second)return true;
    return false;
}
std::ostream &Value::print(std::ostream &os) const { return os << "NULL"; }

std::ostream &IntValue::print(std::ostream &os) const { return os << value; }

std::ostream &DoubleValue::print(std::ostream &os) const {
    return os << fixed << std::setprecision(4) << value;
}

std::ostream &CharValue::print(std::ostream &os) const { return os << value; }

std::ostream &DateValue::print(std::ostream &os) const { return os << get_formated(); }

std::ostream &TimeValue::print(std::ostream &os) const { return os << get_formated(); }

std::ostream &operator<<(std::ostream &os, const Value &v) {
    return v.print(os);
}

ptr_v Value::cast_up(ptr_v v) const { return v; }

ptr_v IntValue::cast_up(ptr_v v) const { return v; }

ptr_v DoubleValue::cast_up(ptr_v v) const {
    if (auto p = dynamic_pointer_cast<IntValue>(v)) {
        return p->to_double();
    }
    return v;
}

ptr_v CharValue::cast_up(ptr_v v) const { return v; }

ptr_v DateValue::cast_up(ptr_v v) const {
    if (auto p = dynamic_pointer_cast<CharValue>(v)) {
        return p->to_date();
    }
    return v;
}

ptr_v TimeValue::cast_up(ptr_v v) const {
    if (auto p = dynamic_pointer_cast<CharValue>(v)) {
        return p->to_time();
    }
    return v;
}

OprdType Value::oprd_type() const { return Null; }

OprdType DoubleValue::oprd_type() const { return Double; }

OprdType IntValue::oprd_type() const { return value ? NZero : Zero; }

OprdType CharValue::oprd_type() const { return Char; }

OprdType DateValue::oprd_type() const { return Date; }

OprdType TimeValue::oprd_type() const { return Time; }

ptr_v Value::v_not(ptr_v x) {
    if (x->oprd_type() == Null) return x;
    if (auto p = dynamic_pointer_cast<IntValue>(x)) {
        return std::make_shared<IntValue>(x->oprd_type() == Zero);
    }
    assert(false);
}

ptr_v Value::v_and(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Zero || y->oprd_type() == Zero) {
        return std::make_shared<IntValue>(0);
    }
    if (x->oprd_type() == Null || y->oprd_type() == Null) {
        return std::make_shared<Value>();
    }
    return std::make_shared<IntValue>(1);
}

ptr_v Value::v_or(ptr_v x, ptr_v y) {
    if (x->oprd_type() == NZero || y->oprd_type() == NZero) {
        return std::make_shared<IntValue>(1);
    }
    if (x->oprd_type() == Null || y->oprd_type() == Null) {
        return std::make_shared<Value>();
    }
    return std::make_shared<IntValue>(0);
}

ptr_v Value::v_xor(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Null || y->oprd_type() == Null) {
        return std::make_shared<Value>();
    }
    bool v1= (x->oprd_type() == NZero);
    bool v2= (y->oprd_type() == NZero);
    return std::make_shared<IntValue>(v1^v2);
}

ptr_v Value::v_lt(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Null) return x;
    if (y->oprd_type() == Null) return y;
    x = y->cast_up(x);
    y = x->cast_up(y);
    return std::make_shared<IntValue>(*x < *y);
}

ptr_v Value::v_gt(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Null) return x;
    if (y->oprd_type() == Null) return y;
    x = y->cast_up(x);
    y = x->cast_up(y);
    return std::make_shared<IntValue>(*y < *x);
}

ptr_v Value::v_eq(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Null) return x;
    if (y->oprd_type() == Null) return y;
    x = y->cast_up(x);
    y = x->cast_up(y);
    return std::make_shared<IntValue>(!(*x < *y) && !(*y < *x));
}

ptr_v Value::v_like(ptr_v x, ptr_v y) {
	if (x->oprd_type() == Null) return x;
	if (y->oprd_type() == Null) return y;
	string pattern((char*)(y->getval()));//Æ¥Åä×Ö·û´®
	if (pattern[0] == '\'' || pattern[0] == '\"') {
		pattern = pattern.substr(1, pattern.size() - 2);
	}
	pattern = utils::toregex(pattern);
	string text((char*)(x->getval()));	//±»ËÑË÷×Ö·û´®
	std::smatch m;
	std::regex e(pattern);
	if (std::regex_match(text, m, e, regex_constants::match_default)) {
		return std::make_shared<IntValue>(1);
	}
	else {
		return std::make_shared<IntValue>(0);
	}
}

ptr_v Value::v_add(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Null) return x;
    if (y->oprd_type() == Null) return y;
    x = y->cast_up(x);
    y = x->cast_up(y);
    if(auto p1 = dynamic_pointer_cast<IntValue>(x)){
        if(auto p2 = dynamic_pointer_cast<IntValue>(y)){
            return std::make_shared<IntValue>(p1->get_value() + p2->get_value());
        }
    }
    if(auto p1 = dynamic_pointer_cast<DoubleValue>(x)){
        if(auto p2 = dynamic_pointer_cast<DoubleValue>(y)){
            return std::make_shared<DoubleValue>(p1->get_value() + p2->get_value());
        }
    }
    if(auto p1 = dynamic_pointer_cast<CharValue>(x)){
        if(auto p2 = dynamic_pointer_cast<CharValue>(y)){
            return std::make_shared<CharValue>(p1->get_val() + p2->get_val());
        }
    }
    return std::make_shared<Value>();
}

ptr_v Value::v_minus(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Null) return x;
    if (y->oprd_type() == Null) return y;
    x = y->cast_up(x);
    y = x->cast_up(y);
    if(auto p1 = dynamic_pointer_cast<IntValue>(x)){
        if(auto p2 = dynamic_pointer_cast<IntValue>(y)){
            return std::make_shared<IntValue>(p1->get_value() - p2->get_value());
        }
    }
    if(auto p1 = dynamic_pointer_cast<DoubleValue>(x)){
        if(auto p2 = dynamic_pointer_cast<DoubleValue>(y)){
            return std::make_shared<DoubleValue>(p1->get_value() - p2->get_value());
        }
    }
    return std::make_shared<Value>();
}

ptr_v Value::v_multiply(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Null) return x;
    if (y->oprd_type() == Null) return y;
    x = y->cast_up(x);
    y = x->cast_up(y);
    if(auto p1 = dynamic_pointer_cast<IntValue>(x)){
        if(auto p2 = dynamic_pointer_cast<IntValue>(y)){
            return std::make_shared<IntValue>(p1->get_value() * p2->get_value());
        }
    }
    if(auto p1 = dynamic_pointer_cast<DoubleValue>(x)){
        if(auto p2 = dynamic_pointer_cast<DoubleValue>(y)){
            return std::make_shared<DoubleValue>(p1->get_value() * p2->get_value());
        }
    }
    return std::make_shared<Value>();
}

ptr_v Value::v_div(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Null) return x;
    if (y->oprd_type() == Null) return y;
    x = y->cast_up(x);
    y = x->cast_up(y);
    if(auto p1 = dynamic_pointer_cast<IntValue>(x)){
        if(auto p2 = dynamic_pointer_cast<IntValue>(y)){
            if(p2->get_value() == 0)return std::make_shared<Value>();
            return std::make_shared<DoubleValue>((double)p1->get_value() / (double)p2->get_value());
        }
    }
    if(auto p1 = dynamic_pointer_cast<DoubleValue>(x)){
        if(auto p2 = dynamic_pointer_cast<DoubleValue>(y)){
            if(p2->get_value() == 0)return std::make_shared<Value>();
            return std::make_shared<DoubleValue>(p1->get_value() / p2->get_value());
        }
    }
    return std::make_shared<Value>();
}

ptr_v Value::v_mod(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Null) return x;
    if (y->oprd_type() == Null) return y;
    x = y->cast_up(x);
    y = x->cast_up(y);
    if(auto p1 = dynamic_pointer_cast<IntValue>(x)){
        if(auto p2 = dynamic_pointer_cast<IntValue>(y)){
            if(p2->get_value() == 0)return std::make_shared<Value>();
            return std::make_shared<IntValue>(p1->get_value() % p2->get_value());
        }

    }
    return std::make_shared<Value>();
}

ptr_v Value::adddate(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Null) return x;
    if (y->oprd_type() == Null) return y;
    if(auto p1 = dynamic_pointer_cast<CharValue>(x)){
        if(auto p2 = dynamic_pointer_cast<IntValue>(y)){
            return std::make_shared<DateValue>(p1->to_date()->get_val() + p2->get_value());
        }
    }
    if(auto p1 = dynamic_pointer_cast<DateValue>(x)){
        if(auto p2 = dynamic_pointer_cast<IntValue>(y)){
            return std::make_shared<DateValue>(p1->get_val() + p2->get_value());
        }
    }
    return std::make_shared<DateValue>(0);
}

ptr_v Value::addtime(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Null) return x;
    if (y->oprd_type() == Null) return y;
    if(auto p1 = dynamic_pointer_cast<CharValue>(x)){
        if(auto p2 = dynamic_pointer_cast<IntValue>(y)){
            return std::make_shared<TimeValue>(p1->to_time()->get_val() + p2->get_value());
        }
    }
    if(auto p1 = dynamic_pointer_cast<TimeValue>(x)){
        if(auto p2 = dynamic_pointer_cast<IntValue>(y)){
            return std::make_shared<TimeValue>(p1->get_val() + p2->get_value());
        }
    }
    return std::make_shared<TimeValue>(0);
}
