#pragma once
#include <map>
#include "Record.h"
#include "SyntaxTree.h"
#include "Expr.h"
#include "Value.h"

#include "Parser.h"

using namespace std;

class WhereClauses{
    shared_ptr<RecordExpr> recordexpr;
public:
    WhereClauses(const string&, const map<string, int>&);
    bool check(const Record &r) const;
};
