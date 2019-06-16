#pragma once
#include <string>
#include <iostream>
#include <memory>
using namespace std;

class Value;
using ptr_v = shared_ptr<Value>;

enum OprdType { Null, Zero, NZero, Double, Char, Date, Time};

constexpr int total_type = Char + 1;

class Value{
    virtual ostream& print(ostream&) const;
public:
    Value()=default;
    virtual ~Value()=default;
    virtual bool operator<(const Value&) const;
	virtual bool operator==(const Value&)const { return false; }
    virtual ptr_v cast_up(ptr_v) const;
    virtual OprdType oprd_type() const;
    friend ostream& operator<<(ostream&, const Value&);
    static ptr_v v_not(ptr_v);
    static ptr_v v_and(ptr_v, ptr_v);
    static ptr_v v_or(ptr_v, ptr_v);
    static ptr_v v_xor(ptr_v, ptr_v);
    static ptr_v v_lt(ptr_v, ptr_v);
    static ptr_v v_gt(ptr_v, ptr_v);
    static ptr_v v_eq(ptr_v, ptr_v);
	static ptr_v v_like(ptr_v, ptr_v);
	static ptr_v v_add(ptr_v, ptr_v);
	static ptr_v v_minus(ptr_v, ptr_v);
    static ptr_v v_multiply(ptr_v, ptr_v);
    static ptr_v v_div(ptr_v, ptr_v);
    static ptr_v v_mod(ptr_v, ptr_v);
    static ptr_v adddate(ptr_v, ptr_v);
    static ptr_v addtime(ptr_v, ptr_v);

	virtual bool isnull() { return true; }
	virtual void* getval() { return nullptr; }
};

class DoubleValue : public Value{
    double value;
    ostream& print(ostream&) const override;
public:
    DoubleValue(double);
	double get_value() const{ return value; }
    bool operator<(const Value&) const override;
	bool operator==(const Value& v) const override { return !(*this < v) && !(v < *this); }
    ptr_v cast_up(ptr_v) const override;
    OprdType oprd_type() const override;
	bool isnull() override { return false; }
	void* getval() override { return (void*)(&value); }
};

class DateValue;
class TimeValue;

class IntValue : public Value{
    int value;
    ostream& print(ostream&) const override;
public:
    IntValue(int);
    shared_ptr<DoubleValue> to_double() const;
	int get_value() const{ return value; }
    bool operator<(const Value&) const override;
	bool operator==(const Value& v) const override { return !(*this < v) && !(v < *this); }
	ptr_v cast_up(ptr_v) const override;
    OprdType oprd_type() const override;
	bool isnull() override { return false; }
	void* getval() override { return (void*)(&value); }
};

class CharValue : public Value{
    string value;
    ostream& print(ostream&) const override;
public:
    CharValue(string);
    shared_ptr<DateValue> to_date() const;
    shared_ptr<TimeValue> to_time() const;
	string get_val() const{ return value; }
    bool operator<(const Value&) const override;
	bool operator==(const Value& v) const override { return !(*this < v) && !(v < *this); }
	ptr_v cast_up(ptr_v) const override;
    OprdType oprd_type() const override;
	bool isnull() override { return false; }
	void* getval() override { return (void*)(value.c_str()); }

};

class DateValue : public Value{
    int year,month,day;
    ostream& print(ostream&) const override;
    void setzero();
    bool isValid();
    bool isLeap(int) const;
public:
    DateValue(string);
    DateValue(int);
    string get_formated() const;
    bool operator<(const Value&) const override;
	bool operator==(const Value& v) const override { return !(*this < v) && !(v < *this); }
    ptr_v cast_up(ptr_v) const override;
    OprdType oprd_type() const override;
    bool isnull() override { return false; }
	void* getval() override { return nullptr; }
	int get_val() const;//以1000-01-01为第1天，直到9999-12-31
};

class TimeValue : public Value{
    int hour,minute,second;
    bool positive=true;
    ostream& print(ostream&) const override;
    void setzero();
    bool isValid();
public:
    TimeValue(string);
    TimeValue(int);
    string get_formated() const;
    bool operator<(const Value&) const override;
	bool operator==(const Value& v) const override { return !(*this < v) && !(v < *this); }
    ptr_v cast_up(ptr_v) const override;
    OprdType oprd_type() const override;
    bool isnull() override { return false; }
	void* getval() override { return nullptr; }
	int get_val() const;//以00:00:00为第1秒
};
