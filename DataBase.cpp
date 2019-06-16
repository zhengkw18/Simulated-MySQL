#include <string>
#include <vector>
#include <map>
#include "utils.h"
#include "DataBase.h"
#include "Table.h"
#include "Attribute.h"
using namespace std;
using namespace utils;

DataBase::DataBase(const string &_name):name(_name){}

string DataBase::get_name() const{
    return name;
}

shared_ptr<Table> DataBase::get_table(const string& name) const {
    return tables.find(name)==tables.end()?NULL:tables.at(name);
}

void DataBase::create_table(const string& name, const vector<Attribute>& attrs,
                            const string& primary_key) {
    tables[name]=make_shared<Table>(name,attrs,primary_key);
}

void DataBase::drop_table(const string& name) {
    tables.erase(name);
}

void DataBase::show_tables() const {
    if(tables.empty()) return;
    cout<<"Tables_in_"<<name<<endl;
    for(auto p:tables){
        cout<<p.second->get_name()<<endl;
    }
}
