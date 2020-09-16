#pragma once
#include <map>
#include <string>
#include <memory>
#include "DataBase.h"

class SQL{
    map<std::string,std::shared_ptr<DataBase>> databases;
    std::shared_ptr<DataBase> current_database;
public:
    SQL()=default;
    std::shared_ptr<DataBase> get_current_database() const;
    std::shared_ptr<DataBase> get_database(const std::string &name) const;
    void drop_database(const std::string &name);
    void create_database(const std::string &name);
    void use(const std::string &name);
    void show_databases() const;
};