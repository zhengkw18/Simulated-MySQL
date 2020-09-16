#pragma once
#include <string>
#include <memory>
#include <sstream>
#include "SQL.h"
#include "DataBase.h"
#include "Table.h"
#include "Record.h"
#include "Group.h"
#include "Value.h"
#include "WhereClauses.h"
#include "Parser.h"
using namespace std;

class Instruction{

public:
    virtual void exec(SQL &sql)=0;
    virtual ~Instruction()=default;
};

/*
specific instructions
Instruction(string) : parse the input (letters are all in lower case)
exec(SQL&) : execute the instruction on the SQL
*/

class CreateDataBaseInst : public Instruction{
    string dbname;
public:
    CreateDataBaseInst()=delete;
    CreateDataBaseInst(string);
    void exec(SQL &sql);
};

class DropDataBaseInst : public Instruction{
    string dbname;
public:
    DropDataBaseInst()=delete;
    DropDataBaseInst(string);
    void exec(SQL &sql);
};

class UseInst : public Instruction{
    string dbname;
public:
    UseInst()=delete;
    UseInst(string);
    void exec(SQL &sql);
};

class ShowDataBasesInst : public Instruction{
    /*no args*/
public:
    ShowDataBasesInst()=default;
    ShowDataBasesInst(string);
    void exec(SQL &sql);
};

class CreateTableInst : public Instruction{
    string tableName;
    vector<Attribute> attrs;
    string primary;
    static Parser ps;
    void parseAttrs();
    void parseAttribute();
public:
    CreateTableInst()=delete;
    CreateTableInst(string);
    void exec(SQL &sql);
};

class DropTableInst : public Instruction{
    string tableName;
public:
    DropTableInst()=delete;
    DropTableInst(string);
    void exec(SQL &sql);
};

class ShowTablesInst : public Instruction{
    /*no args*/
public:
    ShowTablesInst()=default;
    ShowTablesInst(string);
    void exec(SQL &sql);
};

class ShowColumnsFromInst : public Instruction{
    string tableName;
public:
    ShowColumnsFromInst()=delete;
    ShowColumnsFromInst(string);
    void exec(SQL &sql);
};

class InsertIntoInst : public Instruction{
    string tablename;
    vector<string> attrnames, values;
    static Parser ps;

public:
    InsertIntoInst()=delete;
    InsertIntoInst(string);
    void exec(SQL &sql);
};

class DeleteFromInst : public Instruction{
    string tablename, whereclauses_str;
    static Parser ps;

public:
    DeleteFromInst()=delete;
    DeleteFromInst(string);
    void exec(SQL &sql);
};

class UpdateInst : public Instruction {
    string tablename, whereclauses_str;
    string attrname, attrexp;
    static Parser ps;

public:
    UpdateInst()=delete;
    UpdateInst(string);
    void exec(SQL &sql);
};

class LoadDataInst : public Instruction{
    string tablename,filename;
    static Parser ps;
    vector<string> fieldnames;
public:
    LoadDataInst()=delete;
    LoadDataInst(string);
    void exec(SQL &sql);
};

class SelectInst :public Instruction {
    //select XX from XX group by XX order by XX where XX into XX
protected:
    string clauses[6];//分别对应select/from/group by/order by/where/into后的子句
    bool haskeyword[6] = {false, false, false, false, false, false};
    bool selectall = false;
    vector<string> selected;
    vector<string> orders;
    map<string, string> aliases;//字段名与输出名的映射，即别名
    vector<string> tablenames;
	shared_ptr<vTable> vtable;
    static Parser ps;
public:
	SelectInst(string);
	void construct_vtable(SQL& sql);
	shared_ptr<vTable> get_vtable() const {return vtable;}
	vector<string> get_selected() const {return selected;}
	void exec(SQL& sql);
};

class SelectUnionInst :public Instruction {
    //(select XX, XX ……) union (all) (select XX, XX ……) order by XX, XX
protected:
    bool hasorder;
    bool unionall;
    string select1, select2;
    vector<string> orders;
	shared_ptr<vTable> vtable;
    static Parser ps;
public:
	SelectUnionInst(string);
	void exec(SQL& sql);
};

class SelectJoinInst :public Instruction {
    //select x.XX, ,y.XX from a x () join b y on x.XX = y.XX
protected:
    int type;
    string selectclause;
    string table1,table2;
    string aliase1,aliase2;
    string onclausestr;
    bool selectall = false;
    vector<string> selected;
    map<string, string> aliases;//字段名与输出名的映射，即别名
	shared_ptr<vTable> vtable;
    static Parser ps;
public:
	SelectJoinInst(string);
	void exec(SQL& sql);
};
