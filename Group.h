#pragma once
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <algorithm>
#include <cassert>
#include <iostream>
#include "Record.h"
#include "SyntaxTree.h"
using namespace std;

class Group {
	vector<Record> records;
	int value;
public:
	int size() const;
	Record get_record(int record_id) const;
    Group(){}
    Group(const vector<Record>&);
	void addRecord(Record& r);
	void setValue(int v) { value = v; }
	int getValue() const{ return value; }
    shared_ptr<Value> caculatePoland(vector<shared_ptr<Node>>&) const;
    static vector<shared_ptr<Group>> get_group(const vector<Record>& records,int);
};
