#ifndef CURSOR_H_INCLUDED
#define CURSOR_H_INCLUDED
/**
* ָ��`table`��ͷ
* ָ��`table`��β
* ָ��`table`�е�һ��`row`
* ��`cursor`�����ƽ�
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