#include "Attribute.h"
#include "Value.h"
#include "utils.h"
#include <cassert>
#include <memory>
#include <string>

Attribute::Attribute() {}

Attribute::Attribute(const std::string &name, const std::string &type, int id,
                     bool not_null)
    : name(name), type(utils::tolower(type)), id(id), not_null(not_null) {}

int Attribute::get_id() const { return id; }
std::string Attribute::get_name() const { return name; }
bool Attribute::is_not_null() const { return not_null; }

std::shared_ptr<Value> Attribute::create_field(const std::string &value) const {
    if (utils::tolower(value) == "null")
        return std::make_shared<Value>();
    if (type == "char")
        return std::make_shared<CharValue>(value.substr(1, value.length() - 2));
    if (type == "int")
        return std::make_shared<IntValue>(std::stoi(value));
    if (type == "double")
        return std::make_shared<DoubleValue>(std::stod(value));
    if (type == "date")
        return std::make_shared<DateValue>(value.substr(1, value.length() - 2));
    if (type == "time")
        return std::make_shared<TimeValue>(value.substr(1, value.length() - 2));
    assert(false);
}

bool Attribute::operator<(const Attribute &attr) const {
    return name < attr.name;
}

std::string Attribute::get_type() const{
    if(type=="int") return "int(11)";
    if(type=="char") return "char(1)";
    if(type=="double") return "double";
    if(type=="date") return "date";
    if(type=="time") return "time";
}
