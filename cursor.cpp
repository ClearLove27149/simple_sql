#include "cursor.h"

Cursor::Cursor(Table *&table, bool option) {
    this->table = table;
    if (option) {
        row_num = 0;
        end_of_table = (table->num_rows == 0);
    }
    else {
        row_num = table->num_rows;
        end_of_table = true;
    }
}

void* Cursor::cursor_value() {
    // slot_row 函数
    uint32_t page_num = row_num/ROWS_PER_PAGE; // 第几个page

    void* page = table->pager.get_page(page_num);
    // 计算这一页的偏移
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return (char *)page + byte_offset;
}

void Cursor::cursor_advance() {
    row_num++;
    if (row_num >= table->num_rows) {
        end_of_table = true;
    }
}
