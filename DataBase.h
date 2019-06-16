#pragma once
#include <string>
#include <vector>
#include<map>
#include "Table.h"
#include "Attribute.h"

class DataBase{
    std::string name;
    std::map<std::string,std::shared_ptr<Table>> tables;
public:
    DataBase()=delete;
    DataBase(const std::string&);
    std::string get_name() const;
    std::shared_ptr<Table> get_table(const std::string &name) const;
    void create_table(const std::string& name, const std::vector<Attribute>& attrs,
                      const string& primary_key);
    void drop_table(const std::string& name);
    void show_tables() const;
};