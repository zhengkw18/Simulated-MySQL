#include "SQL.h"
#include "DataBase.h"
#include "utils.h"
#include <string>
#include <iostream>
using namespace std;
using namespace utils;

shared_ptr<DataBase> SQL::get_current_database() const {
    return current_database;
}

shared_ptr<DataBase> SQL::get_database(const std::string& name) const {
    return databases.find(name)==databases.end()?NULL:databases.at(name);
}

void SQL::drop_database(const std::string& name) {
    auto it=databases.find(name);
    if(it==databases.end())
        return;
    if(it->second==current_database)
        current_database.reset();
    databases.erase(it);
}

void SQL::create_database(const std::string& name) {
    databases[name] = std::make_shared<DataBase>(name);
}

void SQL::use(const string& name) {
    current_database = databases.at(name);
}

void SQL::show_databases() const {
    if(databases.empty()) return;
    cout << "Database" << endl;
    for (auto p : databases) {
        std::cout << p.second->get_name() << std::endl;
    }
}
