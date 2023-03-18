#pragma once
#ifndef DB_H_INCLUDED
#define DB_H_INCLUDED

#include "table.h"
#include "cursor.h"
#include "index.h"
#include "util.h"

#include <string>
#include <string.h>
#include <iostream>


enum meta_command_result {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
};
// statement
enum prepare_result {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR,
    PREPARE_NEGACTIVE_ID,
    PREPARE_STRING_TOO_LONG
};
//execute
enum execute_result {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
};
//
enum statement_type {
    STATEMENT_INSERT,
    STATEMENT_SELECT,
    STATEMENT_SELECT_USE_ID
};

struct Statement {
    statement_type type;
    Row row_to_insert; // plan to insert row
    Index select_index;
    int condition;
    Statement() = default;
};

class DB {
private:
    Table *table;
    IndexTree *index_tree;
    ::uint32_t cre_id; // 全局自增主键
public:
    DB(const char* db_filename="test.db", const char* index_filename="test.index") {
        table = new Table(db_filename);
        index_tree = new IndexTree(index_filename);
        cre_id = 0;
    }
    ~DB() {
        delete table;
        delete index_tree;
    }
    void start();
    void print_prompt();
    bool parser_meta_command(std::string command);
    meta_command_result do_meta_command(std::string command);
    //
    prepare_result prepare_insert(std::string input_line, Statement& statement);
    prepare_result prepare_select_use_id(std::string input_line, Statement& statement);
    prepare_result prepare_statement(std::string input_line, Statement& statement);
    bool parser_statement(std::string input_line, Statement& statement);
    //execute
    execute_result execute_insert(Statement& statement);
    execute_result execute_insert_use_cursor(Statement& statement);

    execute_result execute_select_use_id(Statement& statement);

    execute_result execute_select(Statement& statement);
    execute_result execute_select_use_cursor(Statement& statement);

    void execute_statement(Statement& statement);
};



#endif // DB_H_INCLUDED
