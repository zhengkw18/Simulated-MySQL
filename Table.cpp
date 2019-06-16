#include <vector>
#include <set>
#include <map>
#include <memory>
#include <string>
#include <cassert>
#include "Table.h"

Table::Table(const string& _name, const vector<Attribute>& _attrs, const string& primary) : name(_name) {
    for(int i = 0; i < _attrs.size(); i++) {
        assert(_attrs[i].get_id() == i);
        id2attr.push_back(_attrs[i]);
        name2id[_attrs[i].get_name()] = _attrs[i].get_id();
        if(_attrs[i].get_name() == primary) {
            primary_id = i;
        }
    }
}

string Table::get_name() {
    return name;
}

bool ValuePtrComparator::operator() (const shared_ptr<Value>& a, const shared_ptr<Value>& b) const {
    return *(a) < *(b);
}

void Table::insert_into(const vector<string>& attrnames, const vector<string>& values) {
    vector<shared_ptr<Value> > ptrs(0);
    map<int, string> pos2value;

    assert(attrnames.size() == values.size());
    for(int i = 0; i < attrnames.size(); i++) {
        assert(name2id.count(attrnames[i]));
        int pos = name2id[attrnames[i]];
        pos2value[pos] = values[i];
    }

    for(int i = 0; i < id2attr.size(); i++) {
        Attribute& cur = id2attr[i];
        if(pos2value.count(i)) {
            ptrs.push_back(cur.create_field(pos2value[i]));
        } else {
            ptrs.push_back(cur.create_field("null"));
        }
    }
    Record rec(ptrs);
    pvr res = pvr(rec.get_field(primary_id), rec);
    records.insert(res);
}

void Table::delete_from(const WhereClauses& wc) {
    for(auto it = records.begin(); it != records.end();) {
        if(wc.check(it->second)) {
            records.erase(it++);
        } else {
            it++;
        }
    }
}

void Table::update(const string& attrname, const string& attrexp, const WhereClauses& wc) {
    Attribute& Attr = id2attr[name2id[attrname]];
    int attr_id = Attr.get_id();
    if(attr_id == primary_id) { // keep map ordered
        vector<Record> buffer;
        for(auto it = records.begin(); it != records.end();) {
            if(wc.check(it->second)) {
                buffer.push_back(it->second);
                records.erase(it++);
            } else {
                it++;
            }
        }
        for(Record& cur : buffer) {
            cur.update(attr_id, Attr.create_field(attrexp));
            records.insert(pvr(cur.get_field(attr_id), cur));
        }
    } else { // simply update
        for(auto& it : records) {
            if(wc.check(it.second)) {
                it.second.update(attr_id, Attr.create_field(attrexp));
            }
        }
    }
}

vector<Record> Table::select(const WhereClauses& wc) {
    vector<Record> res;
    for(auto& it : records) {
        if(wc.check(it.second)) {
            res.push_back(it.second);
        }
    }
    return res;
}

vector<Attribute>& Table::export_id2attr() {
    return id2attr;
}
map<string, int>& Table::export_name2id() {
    return name2id;
}

void Table::show_columns() const{
    cout<<"Field\tType\tNull\tKey\tDefault\tExtra"<<endl;
    for(auto &attr:id2attr){
        cout<<attr.get_name()<<'\t';
        cout<<attr.get_type()<<'\t';
        cout<<(attr.is_not_null()?"NO":"YES")<<'\t';
        cout<<(attr.get_id()==primary_id?"PRI":"")<<'\t';
        cout<<"NULL"<<'\t';
        cout<<endl;
    }
}

shared_ptr<vTable> Table::get_vTable(){
    shared_ptr<vTable> vtable = make_shared<vTable>(id2attr);
    for(auto& it : records) {
        vtable->insert_into(it.second);
    }
    return vtable;
}
