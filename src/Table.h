#pragma once
#include "Record.h"
#include "Attribute.h"
#include "Value.h"
#include "WhereClauses.h"
#include <set>
#include <map>
#include <memory>
#include <vector>
#include <cassert>
#include "vTable.h"
using namespace std;

typedef pair<shared_ptr<Value>, Record> pvr;

class ValuePtrComparator {
public:
    bool operator() (const shared_ptr<Value>&, const shared_ptr<Value>&) const;
};

class Table{
    /*
    * attribute
    * id to attribute
    * records
    * primary key
    */

    string name;
    vector<Attribute> id2attr;
    map<string, int> name2id;
    int primary_id;

    multimap<shared_ptr<Value>, Record, ValuePtrComparator> records;

public:
    Table(const string&, const vector<Attribute>&, const string&);
    string get_name();
    void show_columns() const;
    void insert_into(const vector<string>&, const vector<string>&);
    void delete_from(const WhereClauses&);
    void update(const string&, const string&, const WhereClauses&);
    vector<Record> select(const WhereClauses&);
    vector<Attribute>& export_id2attr();
    map<string, int>& export_name2id();
    shared_ptr<vTable> get_vTable();
};
