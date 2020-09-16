#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstring>
#include <assert.h>
#include "utils.h"
#include "Group.h"
#include "Instruction.h"
#include "Expr.h"
using namespace std;
SelectUnionInst::SelectUnionInst(string info){
    string lowinfo = utils::tolower(info);
    int unionpos = lowinfo.find(" union ");
    int orderpos = lowinfo.find(" order by ");
    hasorder = (orderpos != string::npos);
    unionall = (lowinfo.find(" union all ") != string::npos);
    if(hasorder){
        string orderstr = info.substr(orderpos+10, info.length()-orderpos-10);
        stringstream ss(orderstr);
        string order;
        while(ss>>order)orders.push_back(order);
    }else{
        orderpos = info.length();
    }
    select1 = info.substr(0, unionpos);
    if(unionall){
        select2 = info.substr(unionpos+11, orderpos-unionpos-11);
    }else{
        select2 = info.substr(unionpos+7, orderpos-unionpos-7);
    }
}

void SelectUnionInst::exec(SQL& sql){
    SelectInst si1(select1);
    SelectInst si2(select2);
    si1.construct_vtable(sql);
    si2.construct_vtable(sql);
    shared_ptr<vTable> u = vTable::unionvtable(si1.get_vtable(), si2.get_vtable(), si1.get_selected(), si2.get_selected());
    if(!unionall) u->deleteRepeated();
    if(hasorder) u->order(orders);
    u->outputall(cout, true);
}
