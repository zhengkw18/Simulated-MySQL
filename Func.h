#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include <memory>
#include "utils.h"
using namespace std;
namespace RecordFunc{
    static map<string, function<shared_ptr<Value>(vector<shared_ptr<Value>>)>>
    name_expr={
    {"char_length", [](vector<shared_ptr<Value>> values) -> shared_ptr<Value>{
        if(auto p = dynamic_pointer_cast<CharValue>(values[0])){
            return make_shared<IntValue>(p->get_val().length());
        }
        return make_shared<IntValue>(0);
    }
    },

    {"lower", [](vector<shared_ptr<Value>> values) -> shared_ptr<Value>{
        if(auto p = dynamic_pointer_cast<CharValue>(values[0])){
            return make_shared<CharValue>(utils::tolower(p->get_val()));
        }
        return make_shared<CharValue>("");
    }
    },

    {"upper", [](vector<shared_ptr<Value>> values) -> shared_ptr<Value>{
        if(auto p = dynamic_pointer_cast<CharValue>(values[0])){
            return make_shared<CharValue>(utils::toupper(p->get_val()));
        }
        return make_shared<CharValue>("");
    }
    },

    {"space", [](vector<shared_ptr<Value>> values) -> shared_ptr<Value>{
        if(auto p = dynamic_pointer_cast<IntValue>(values[0])){
            string str(p->get_value(), ' ');
            return make_shared<CharValue>(str);
        }
        return make_shared<CharValue>("");
    }
    },

    {"abs", [](vector<shared_ptr<Value>> values) -> shared_ptr<Value>{
        if(auto p = dynamic_pointer_cast<IntValue>(values[0])){
            return p->get_value()<0 ? make_shared<IntValue>(-p->get_value()) : make_shared<IntValue>(p->get_value());
        }
        if(auto p = dynamic_pointer_cast<DoubleValue>(values[0])){
            return p->get_value()<0 ? make_shared<DoubleValue>(-p->get_value()) : make_shared<DoubleValue>(p->get_value());
        }
        return make_shared<IntValue>(0);
    }
    },

    {"sin", [](vector<shared_ptr<Value>> values) -> shared_ptr<Value>{
        if(auto p = dynamic_pointer_cast<IntValue>(values[0])){
            return make_shared<DoubleValue>(sin(p->get_value()));
        }
        if(auto p = dynamic_pointer_cast<IntValue>(values[0])){
            return make_shared<DoubleValue>(sin(p->get_value()));
        }
        return make_shared<DoubleValue>(0);
    }
    },

    {"cos", [](vector<shared_ptr<Value>> values) -> shared_ptr<Value>{
        if(auto p = dynamic_pointer_cast<IntValue>(values[0])){
            return make_shared<DoubleValue>(cos(p->get_value()));
        }
        if(auto p = dynamic_pointer_cast<IntValue>(values[0])){
            return make_shared<DoubleValue>(cos(p->get_value()));
        }
        return make_shared<DoubleValue>(0);
    }
    },

    {"tan", [](vector<shared_ptr<Value>> values) -> shared_ptr<Value>{
        if(auto p = dynamic_pointer_cast<IntValue>(values[0])){
            return make_shared<DoubleValue>(tan(p->get_value()));
        }
        if(auto p = dynamic_pointer_cast<IntValue>(values[0])){
            return make_shared<DoubleValue>(tan(p->get_value()));
        }
        return make_shared<DoubleValue>(0);
    }
    },

    {"exp", [](vector<shared_ptr<Value>> values) -> shared_ptr<Value>{
        if(auto p = dynamic_pointer_cast<IntValue>(values[0])){
            return make_shared<DoubleValue>(exp(p->get_value()));
        }
        if(auto p = dynamic_pointer_cast<IntValue>(values[0])){
            return make_shared<DoubleValue>(exp(p->get_value()));
        }
        return make_shared<DoubleValue>(0);
    }
    },

    {"adddate", [](vector<shared_ptr<Value>> values) -> shared_ptr<Value>{
        return Value::adddate(values[0], values[1]);
    }
    },

    {"addtime", [](vector<shared_ptr<Value>> values) -> shared_ptr<Value>{
        return Value::addtime(values[0], values[1]);
    }
    }

    };
    static map<string, int>
    param_num={
    {"char_length", 1},
    {"lower", 1},
    {"upper", 1},
    {"space", 1},
    {"abs", 1},
    {"sin", 1},
    {"cos", 1},
    {"tan", 1},
    {"exp", 1},
    {"adddate", 2},
    {"addtime", 2}
    };
    inline bool hasFunc(string s) {return name_expr.count(s);}
}
namespace GroupFunc{
    static map<string, function<shared_ptr<Value>(vector<shared_ptr<Value>>)>>
    name_expr={
    {"avg", [](vector<shared_ptr<Value>> values) -> shared_ptr<Value>{
        double d=0;
        for(int i=0;i<values.size();i++){
            if(auto p = dynamic_pointer_cast<IntValue>(values[i])){
                d+=p->get_value();
            }
            if(auto p = dynamic_pointer_cast<DoubleValue>(values[i])){
                d+=p->get_value();
            }
        }
        return make_shared<DoubleValue>(d / values.size());
    }
    },

    {"count", [](vector<shared_ptr<Value>> values) -> shared_ptr<Value>{
        int sum=0;
        for(int i=0;i<values.size();i++){
            if(values[i]->oprd_type() != Null)sum++;
        }
        return make_shared<IntValue>(sum);
    }
    },

    {"max", [](vector<shared_ptr<Value>> values) -> shared_ptr<Value>{
        if(auto p = dynamic_pointer_cast<IntValue>(values[0])){
            int m=p->get_value();
            for(int i=1;i<values.size();i++){
                if(auto p1 = dynamic_pointer_cast<IntValue>(values[i])){
                    if(m<p1->get_value())m=p1->get_value();
                }
            }
            return make_shared<IntValue>(m);
        }
        if(auto p = dynamic_pointer_cast<DoubleValue>(values[0])){
            double m=p->get_value();
            for(int i=1;i<values.size();i++){
                if(auto p1 = dynamic_pointer_cast<DoubleValue>(values[i])){
                    if(m<p1->get_value())m=p1->get_value();
                }
            }
            return make_shared<DoubleValue>(m);
        }
        return make_shared<IntValue>(0);
    }
    },

    {"min", [](vector<shared_ptr<Value>> values) -> shared_ptr<Value>{
        if(auto p = dynamic_pointer_cast<IntValue>(values[0])){
            int m=p->get_value();
            for(int i=1;i<values.size();i++){
                if(auto p1 = dynamic_pointer_cast<IntValue>(values[i])){
                    if(m>p1->get_value())m=p1->get_value();
                }
            }
            return make_shared<IntValue>(m);
        }
        if(auto p = dynamic_pointer_cast<DoubleValue>(values[0])){
            double m=p->get_value();
            for(int i=1;i<values.size();i++){
                if(auto p1 = dynamic_pointer_cast<DoubleValue>(values[i])){
                    if(m>p1->get_value())m=p1->get_value();
                }
            }
            return make_shared<DoubleValue>(m);
        }
        return make_shared<IntValue>(0);
    }
    }

    };

    inline bool hasFunc(string s) {return name_expr.count(s) || s == "countall";}
};
