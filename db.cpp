#include "db.h"


void DB::print_prompt() {
    std::cout << "db > ";
}

bool DB::parser_meta_command(std::string command) {
    // command start with '.' is a meta command
    if (command[0] == '.') {
        switch (do_meta_command(command)) {
        case META_COMMAND_SUCCESS:
            return true;
        case META_COMMAND_UNRECOGNIZED_COMMAND:
            std::cout << "Unrecognized command: " << command << std::endl;
            return true;
        }
    }
    return false;
}

meta_command_result DB::do_meta_command(std::string command) {
    if (command == ".exit" || command == ".quit") {
        delete(table); //call ~table()
        //index
        delete(index_tree);
        std::cout << "Bye!" <<std::endl;
        exit(EXIT_SUCCESS);
    } else if (command == ".indextree") {
        index_tree->traverse();
        return META_COMMAND_SUCCESS;
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}
//prepare
prepare_result DB::prepare_insert(std::string input_line, Statement& statement) {
    statement.type = STATEMENT_INSERT;
    char* insert_line = (char *)input_line.c_str();
    char* keyword = strtok(insert_line, " ");
    char* id_string = strtok(NULL, " ");
    char* username = strtok(NULL, " ");
    char* email = strtok(NULL, " ");

    if(id_string == NULL || username == NULL || email == NULL) {
        return PREPARE_SYNTAX_ERROR;
    }
    uint32_t id = atoi(id_string);
    if (id < 0) {
        return PREPARE_NEGACTIVE_ID;
    }
    if (strlen(username) > COLUMN_USERNAME_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }
    if (strlen(email) > COLUMN_EMAIL_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }
    statement.row_to_insert = Row(id, username, email);
    return PREPARE_SUCCESS;

}
prepare_result DB::prepare_select_use_id(std::string input_line, Statement &statement) {
    statement.type = STATEMENT_SELECT_USE_ID;
    vector<string> res = split(input_line, ' ');
    string where = res[res.size()-1];
    //std::cout<<"where: "<<where<<std::endl;
    //
    if (contain(where, "<=")) {
        size_t pos = where.find("<=");
        string id_string = where.substr(pos+2);
        ::uint32_t id = atoi(id_string.c_str());
        statement.condition = -2;
        statement.select_index = Index(id);
    } else if (contain(where, ">=")) {
        size_t pos = where.find(">=");
        string id_string = where.substr(pos+2);
        ::uint32_t id = atoi(id_string.c_str());
        statement.condition = 2;
        statement.select_index = Index(id);
    } else if (contain(where, "<")) {
        size_t pos = where.find("<");
        string id_string = where.substr(pos+1);
        ::uint32_t id = atoi(id_string.c_str());
        statement.condition = -1;
        statement.select_index = Index(id);
    } else if (contain(where, ">")) {
        size_t pos = where.find(">");
        string id_string = where.substr(pos+1);
        ::uint32_t id = atoi(id_string.c_str());
        statement.condition = 1;
        statement.select_index = Index(id);
    } else if (contain(where, "=")) {
        size_t pos = where.find("=");
        string id_string = where.substr(pos+1);
        ::uint32_t id = atoi(id_string.c_str());
        statement.condition = 0;
        statement.select_index = Index(id);
    } else {
        return PREPARE_SYNTAX_ERROR;
    }
    return PREPARE_SUCCESS;
}

prepare_result DB::prepare_statement(std::string input_line, Statement& statement) {
    if (!input_line.compare(0, 6, "insert")) {
        return prepare_insert(input_line, statement);
        /**
        statement.type = STATEMENT_INSERT;
        // 1. 查看参数
        // 2. 将参数写入statement的row_to_insert
        int args_assigned = std::sscanf(input_line.c_str(), "insert %d %s %s", &(statement.row_to_insert.id),
                                        statement.row_to_insert.username, statement.row_to_insert.email);
        if (args_assigned < 3) {
            return PREPARE_SYNTAX_ERROR;
        }

        return PREPARE_SUCCESS;
        */
    } else if (!input_line.compare(0, 6, "select") && input_line.length() == 6) {
        statement.type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    } else if (!input_line.compare(0, 6, "select") && input_line.length() > 6) {
        return prepare_select_use_id(input_line, statement);
    } else {
        return PREPARE_UNRECOGNIZED_STATEMENT;
    }
}
//parse
bool DB::parser_statement(std::string input_line, Statement& statement) {
    switch (prepare_statement(input_line, statement)) {
    case PREPARE_SUCCESS:
        return false;
    case (PREPARE_NEGACTIVE_ID):
        std::cout << "ID must be positive." << std::endl;
        return true;
    case (PREPARE_STRING_TOO_LONG):
        std::cout << "String is too long." << std::endl;
        return true;
    case PREPARE_SYNTAX_ERROR:
        std::cout << "Syntax error. Could not parse statement." << std::endl;
        return true;
    case PREPARE_UNRECOGNIZED_STATEMENT:
        std::cout << "Unrecognized keyword at start of '" << input_line << "'." << std::endl;
        return true;
    }
    return false;
}
//execute
execute_result DB::execute_insert(Statement& statement) {
    if (table->num_rows >= TABLE_MAX_ROWS) {
        std::cout << "Error: Table full" << std::endl;
        return EXECUTE_TABLE_FULL;
    }
    void* page = table->row_slot(table->num_rows);
    Index index(cre_id++, reinterpret_cast<::uint64_t>(page));
    index_tree->insert_index(index);
    serialize_row(statement.row_to_insert, page);
    table->num_rows++;

    return EXECUTE_SUCCESS;
}
execute_result DB::execute_insert_use_cursor(Statement& statement) {
    if (table->num_rows >= TABLE_MAX_ROWS) {
        std::cout << "Error: Table full" << std::endl;
        return EXECUTE_TABLE_FULL;
    }
    Cursor* cursor = new Cursor(table, false);
    void* page = cursor->cursor_value();
    //std::cout << "insert pos: " << reinterpret_cast<::uint64_t>(page) <<std::endl;
    Index index(cre_id++, reinterpret_cast<::uint64_t>(page));
    index_tree->insert_index(index);
    serialize_row(statement.row_to_insert, page);
    table->num_rows++;

    delete cursor;
    return EXECUTE_SUCCESS;
}

execute_result DB::execute_select(Statement& statement) {
    Row row;

    //while(table.num_rows++ < tep_num) {
    for (uint32_t i = 0; i < table->num_rows; i++) {
        void* page = table->row_slot(i);
        //std::cout<<((char*)page)<<std::endl;
        unserialize_row(row, page);
        std::cout << "(" << row.id << ", " << row.username << ", " << row.email << ")" << std::endl;
        memset(&row, 0, sizeof(row));
    }
    return EXECUTE_SUCCESS;
}
execute_result DB::execute_select_use_cursor(Statement& statement) {
    Row row;
    Cursor* cursor = new Cursor(table, true);
    while (!cursor->end_of_table) {
        void* page = cursor->cursor_value();
        unserialize_row(row, page);
        std::cout << "(" << row.id << ", " << row.username << ", " << row.email << ")" << std::endl;
        memset(&row, 0, sizeof(row));

        cursor->cursor_advance(); // next
    }

    delete cursor;
    return EXECUTE_SUCCESS;
}

execute_result DB::execute_select_use_id(Statement &statement) {
    Row row;
    if (statement.condition == 0) { // =
        ::uint64_t pos = index_tree->get_pos(statement.select_index);
        //std::cout << "pos: " <<std::endl;
        if (pos != 0) {
            unserialize_row(row, (void*)pos);
            std::cout << "(" <<row.id<<", "<<row.username<<", "<<row.email<<")"<<std::endl;
        }
    } else {
        //todo
    }
    return EXECUTE_SUCCESS;
}

void DB::execute_statement(Statement& statement) {

    execute_result result;
    switch (statement.type) {
    case STATEMENT_INSERT:
        result = execute_insert_use_cursor(statement);
        //std::cout << "execute insert statement" <<std::endl;
        break;
    case STATEMENT_SELECT:
        result = execute_select_use_cursor(statement);
        //std::cout << "execute select statement" <<std::endl;
        break;
    case STATEMENT_SELECT_USE_ID:
        result = execute_select_use_id(statement);
        break;
    }
    // 判断结果
    switch (result) {
    case EXECUTE_SUCCESS:
        std::cout << "Execute." << std::endl;
        break;

    case EXECUTE_TABLE_FULL:
        std::cout << "Error: Table full" <<std::endl;
        break;
    default:
        std::cout << "Default." <<std::endl;
        break;
    }
}
//
void DB::start() {
    while (true) {
        print_prompt();
        std::string input_line;
        std::getline(std::cin, input_line);
        if (parser_meta_command(input_line)) {  // if parser_meta_command return true, means input_line is not good
            continue;
        }

        Statement statement;
        if (parser_statement(input_line, statement)) {
            continue;
        }
        execute_statement(statement);
    }
}
