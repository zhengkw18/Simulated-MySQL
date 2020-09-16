#include "vTable.h"
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <string>
#include <cassert>
#include <assert.h>
using namespace std;

vTable::vTable(const vector<Attribute>& attributes){
    for(int i=0 ; i<attributes.size();i++){
        id2attr.push_back(attributes[i]);
        name2id[attributes[i].get_name()] = i;
    }
}
void vTable::insert_into(Record& record){
    records.push_back(record);
}

Record& vTable::getRecord(int id){
    return records[id];
}

int vTable::addColumn(string& name){
    if(name2id.count(name))return name2id[name];
    RecordExpr r(name, name2id);
    vector<shared_ptr<Value>> values;
    for(Record record : records){
        values.push_back(r.get_value(record));
    }
    Attribute attribute(name, "int", id2attr.size(), false);
    id2attr.push_back(attribute);
    name2id[name] = id2attr.size()-1;
    for(int i=0; i<values.size(); i++)
        records[i].addValue(values[i]);
    return name2id[name];
}

void vTable::addColumn(string& name, vector<shared_ptr<Value>>& values){
    assert(values.size() == records.size());
    Attribute attribute(name, "int", id2attr.size(), false);
    id2attr.push_back(attribute);
    name2id[attribute.get_name()] = id2attr.size() - 1;
    for(int i=0; i<values.size(); i++)
        records[i].addValue(values[i]);
}

shared_ptr<vTable> vTable::getEmpty(){
    Attribute attr("*", "int", 0, true);
    vector<Attribute> attrs;
    attrs.push_back(attr);
    shared_ptr<Value> value = make_shared<Value>();
    vector<shared_ptr<Value>> values;
    values.push_back(value);
    Record record(values);
    shared_ptr<vTable> emp = make_shared<vTable>(attrs);
    emp->insert_into(record);
    return emp;
}

vector<Attribute>& vTable::export_id2attr() {
    return id2attr;
}
map<string, int>& vTable::export_name2id() {
    return name2id;
}

void vTable::applywhere(const WhereClauses& wc) {
    for(auto it = records.begin(); it != records.end();) {
        if(wc.check(*it)) {
            it++;
        } else {
            it = records.erase(it);
        }
    }
}

shared_ptr<vTable> vTable::getCartesian(shared_ptr<vTable> t1, shared_ptr<vTable> t2, string s1, string s2){
    vector<Attribute> attrs;
    int id=0;
    for(auto attr1 : t1->id2attr){
        if(s1.empty()){
            attrs.push_back(Attribute(attr1.get_name(), attr1.get_type(), id, attr1.is_not_null()));
        }else{
            attrs.push_back(Attribute(s1 + "." + attr1.get_name(), attr1.get_type(), id, attr1.is_not_null()));
        }
        id++;
    }
    for(auto attr2 : t2->id2attr){
        if(s2.empty()){
            attrs.push_back(Attribute(attr2.get_name(), attr2.get_type(), id, attr2.is_not_null()));
        }else{
            attrs.push_back(Attribute(s2 + "." + attr2.get_name(), attr2.get_type(), id, attr2.is_not_null()));
        }
        id++;
    }
    shared_ptr<vTable> vtable= make_shared<vTable>(attrs);
    vector<shared_ptr<Value>> values;
    for(Record r1 : t1->records){
        for(Record r2 : t2->records){
            values.clear();
            for(int i=0;i<r1.size();i++) values.push_back(r1.get_field(i));
            for(int j=0;j<r2.size();j++) values.push_back(r2.get_field(j));
            Record record(values);
            vtable->insert_into(record);
        }
    }
    return vtable;
}

void vTable::output(vector<string>& selected, map<string, string>& aliases, ostream& out, bool hasheader){
    if(hasheader && !records.empty()){
        for(string s :selected){
            string name = aliases[s];
            utils::string_replace(name, "all", "(*)");
            out<<name<<"\t";
        }
        out<<endl;
    }
    for(Record record : records){
        for(string s : selected){
            out<<*record.get_field(name2id[s])<<"\t";
        }
        out<<endl;
    }
}

void vTable::outputall(ostream& out, bool hasheader){
    if(hasheader && !records.empty()){
        for(Attribute attr :id2attr){
            string name = attr.get_name();
            utils::string_replace(name, "all", "(*)");
            out<<name<<"\t";
        }
        out<<endl;
    }

    for(Record record : records){
        for(int i=0;i<id2attr.size();i++){
            out<<*record.get_field(i)<<"\t";
        }
        out<<endl;
    }
}

vector<shared_ptr<Group>> vTable::getGrouped(string& groupby){
    int id = addColumn(groupby);
    vector<string> orders;
    orders.push_back(groupby);
    order(orders);
    return Group::get_group(records, id);
}

void vTable::order(vector<string>& orders){
    vector<int> ids;
    for(string s : orders){
        ids.push_back(name2id[s]);
    }
    sort(records.begin(), records.end(), RecordMultipleComparator(ids));
}

RecordMultipleComparator::RecordMultipleComparator(vector<int>& o){
    for(int i : o)orders.push_back(i);
}

bool RecordMultipleComparator::operator()(const Record& r1, const Record& r2){
    for(int i : orders){
        if(*r1.get_field(i) < *r2.get_field(i))return true;
        if(*r2.get_field(i) < *r1.get_field(i))return false;
    }
    return true;
}

shared_ptr<Group> vTable::getWholeAsGroup(){
    return make_shared<Group>(records);
}
vector<string> vTable::export_id2name(){
    vector<string> names;
    for(Attribute attr: id2attr){
        names.push_back(attr.get_name());
    }
    return names;
}

shared_ptr<vTable> vTable::unionvtable(shared_ptr<vTable> t1, shared_ptr<vTable> t2, vector<string> se1, vector<string> se2){
    if(se1.empty())se1=t1->export_id2name();
    if(se2.empty())se2=t2->export_id2name();
    assert(se1.size() == se2.size());
    vector<Attribute> v;
    for(int i=0;i<se1.size();i++)v.push_back(Attribute(se1[i], "int", 1, false));
    shared_ptr<vTable> vtable = make_shared<vTable>(v);
    vector<shared_ptr<Value>> values;
    map<string, int> name2id1 = t1->export_name2id();
    map<string, int> name2id2 = t2->export_name2id();
    for(Record r : t1->records){
        values.clear();
        for(string name : se1){
            values.push_back(r.get_field(name2id1[name]));
        }
        Record record(values);
        vtable->insert_into(record);
    }
    for(Record r : t2->records){
        values.clear();
        for(string name : se1){
            values.push_back(r.get_field(name2id2[name]));
        }
        Record record(values);
        vtable->insert_into(record);
    }
    return vtable;
}

void vTable::deleteRepeated(){
    for(int i=0;i<records.size();i++){
        auto it=records.begin()+i+1;
        while(it!=records.end()){
            if(*it == records[i]){
                it = records.erase(it);
            }else{
                it++;
            }
        }
    }
}

shared_ptr<vTable> vTable::jointable(shared_ptr<vTable> t1, shared_ptr<vTable> t2, string s1, string s2, string& onclausestr,int type){
    vector<Attribute> attrs;
    int id=0;
    int left=0, right=0;
    for(auto attr1 : t1->id2attr){
        attrs.push_back(Attribute(s1 + "." + attr1.get_name(), attr1.get_type(), id, attr1.is_not_null()));
        id++;
        left++;
    }
    for(auto attr2 : t2->id2attr){
        attrs.push_back(Attribute(s2 + "." + attr2.get_name(), attr2.get_type(), id, attr2.is_not_null()));
        id++;
        right++;
    }
    shared_ptr<vTable> vtable= make_shared<vTable>(attrs);
    WhereClauses onclause(onclausestr, vtable->export_name2id());
    vector<shared_ptr<Value>> values;
    if(type == 0){
        for(Record r1 : t1->records){
            for(Record r2 : t2->records){
                values.clear();
                for(int i=0;i<left;i++) values.push_back(r1.get_field(i));
                for(int j=0;j<right;j++) values.push_back(r2.get_field(j));
                Record record(values);
                if(onclause.check(record))vtable->insert_into(record);
            }
        }
    }else if(type == 1){
        for(Record r1 : t1->records){
            bool matched = false;
            for(Record r2 : t2->records){
                values.clear();
                for(int i=0;i<left;i++) values.push_back(r1.get_field(i));
                for(int j=0;j<right;j++) values.push_back(r2.get_field(j));
                Record record(values);
                if(onclause.check(record)){
                    vtable->insert_into(record);
                    matched = true;
                }
            }
            if(!matched){
                values.clear();
                for(int i=0;i<left;i++) values.push_back(r1.get_field(i));
                for(int j=0;j<right;j++) values.push_back(make_shared<Value>());
                Record record(values);
                vtable->insert_into(record);
            }
        }
    }else if(type == 2){
        for(Record r2 : t2->records){
            bool matched = false;
            for(Record r1 : t1->records){
                values.clear();
                for(int i=0;i<left;i++) values.push_back(r1.get_field(i));
                for(int j=0;j<right;j++) values.push_back(r2.get_field(j));
                Record record(values);
                if(onclause.check(record)){
                    vtable->insert_into(record);
                    matched = true;
                }
            }
            if(!matched){
                values.clear();
                for(int i=0;i<left;i++) values.push_back(make_shared<Value>());
                for(int j=0;j<right;j++) values.push_back(r2.get_field(j));
                Record record(values);
                vtable->insert_into(record);
            }
        }
    }
    return vtable;
}
