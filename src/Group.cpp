#include "Group.h"
#include <memory>
#include <vector>
#include <stack>

using namespace std;
int Group::size() const {
	return records.size();
}

Record Group::get_record(int id) const{
	return records[id];
}

void Group::addRecord(Record& r) {
	records.push_back(r);
}

shared_ptr<Value> Group::caculatePoland(vector<shared_ptr<Node>>& poland) const{
    int num = size();
    stack<vector<shared_ptr<Value>>> columnvalues;
    for(auto node : poland){
        if(node->isOptr()){
            auto optrnode = dynamic_pointer_cast<OptrNode>(node);
            if(optrnode->isFunc()){
                auto func=dynamic_pointer_cast<FuncNode>(optrnode);
                if(func->isGroupFunc()){
                    if(func->is_countall()){
                        vector<shared_ptr<Value>> columnvalue(num, make_shared<IntValue>(num));
                        columnvalues.push(columnvalue);
                    }else{
                        auto params = columnvalues.top();
                        columnvalues.pop();
                        vector<shared_ptr<Value>> columnvalue(num, func->caculate(params));
                        columnvalues.push(columnvalue);
                    }

                }else{
                    vector<vector<shared_ptr<Value>>> paramcolumnvalues;
                    paramcolumnvalues.clear();
                    int paramnum=func->get_paramnum();
                    while(paramnum--){
                        paramcolumnvalues.push_back(columnvalues.top());
                        columnvalues.pop();
                    }
                    reverse(paramcolumnvalues.begin(), paramcolumnvalues.end());

                    vector<shared_ptr<Value>> columnvalue;
                    columnvalue.clear();
                    for(int i=0;i<num;i++){
                        vector<shared_ptr<Value>> params;
                        params.clear();
                        for(int j=0;j<columnvalue.size();j++){
                            params.push_back(paramcolumnvalues[i][j]);
                        }
                        columnvalue.push_back(func->caculate(params));
                    }
                    columnvalues.push(columnvalue);
                }
            }else{
                auto optr = dynamic_pointer_cast<Optr>(optrnode);
                if(optr->isBinary()){
                    auto v2 = columnvalues.top();
                    columnvalues.pop();
                    auto v1 = columnvalues.top();
                    columnvalues.pop();
                    vector<shared_ptr<Value>> columnvalue;
                    columnvalue.clear();
                    for(int i=0;i<num;i++){
                        columnvalue.push_back(optr->caculate(v1[i], v2[i]));
                    }
                    columnvalues.push(columnvalue);
                }else{
                    auto v = columnvalues.top();
                    columnvalues.pop();
                    vector<shared_ptr<Value>> columnvalue;
                    columnvalue.clear();
                    for(int i=0;i<num;i++){
                        columnvalue.push_back(optr->caculate(v[i]));
                    }
                    columnvalues.push(columnvalue);
                }
            }
        }else{
            auto opnd = dynamic_pointer_cast<OpndNode>(node);
            vector<shared_ptr<Value>> columnvalue;
            columnvalue.clear();
            for(int i=0;i<num;i++){
                columnvalue.push_back(opnd->get_value(records[i]));
            }
            columnvalues.push(columnvalue);
        }
    }
    return columnvalues.top()[0];

}

Group::Group(const vector<Record>& res){
    for(auto re : res) records.push_back(re);
}

vector<shared_ptr<Group>> Group::get_group(const vector<Record>& records,int keyid) {
	vector<shared_ptr<Group>> groups;
	int first = 0, tmp = 1,len=records.size(),ngroup=0;
	if (records.size() > 0) {
		while (tmp < len) {
			groups.push_back(make_shared<Group>());
			Record record(records[first]);
			groups[ngroup]->addRecord(record);
			tmp = first + 1;
			while (tmp < len
				&& *(records[tmp].get_field(keyid)) == *(records[first].get_field(keyid))) {

                Record record(records[tmp++]);
				groups[ngroup]->addRecord(record);
			}
			ngroup++;
			first = tmp;
		}
	}
	return groups;
}
