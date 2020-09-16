#pragma once
#include <string>
#include <memory>
#include "Value.h"
using namespace std;

class Attribute{
    string name, type;
    int id;
    bool not_null;
public:
    Attribute(); // Attribute need an empty constructor to initialize 'primary_key'
    Attribute(const string& name, const string& type, int id, bool not_null);
    int get_id() const;
    string get_name() const;
    string get_type() const;
    bool is_not_null() const;
    shared_ptr<Value> create_field(const string &value) const;
    bool operator<(const Attribute&) const;
};
