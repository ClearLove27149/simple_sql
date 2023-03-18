#ifndef CURSOR_H_INCLUDED
#define CURSOR_H_INCLUDED
/**
* 指向`table`开头
* 指向`table`结尾
* 指向`table`中的一个`row`
* 将`cursor`往后推进
**/
#include "table.h"

class Cursor {
public:
    Table *table;
    uint32_t row_num;
    bool end_of_table;

    Cursor(Table *&table, bool option);
    void* cursor_value();
    void cursor_advance();
};
#endif // CURSOR_H_INCLUDED
