#include "utils.h"
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include "WhereClauses.h"

WhereClauses::WhereClauses(const std::string &s,	//原始的where子句
                           const std::map<std::string, int> &name2id) {
    recordexpr = make_shared<RecordExpr>(s, name2id);
}

bool WhereClauses::check(const Record &r) const {
    return recordexpr->get_value(r)->oprd_type() == NZero;
}
