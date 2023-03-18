#pragma once
#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include <memory.h>
#include <malloc.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

#if defined(_WIN32) || defined(_WIN64)
#include <stdio.h>
#else
#include <fcntl.h>
#endif
/**
clumn          size
id              int
username        varchar 32
email           varchar 255
*/
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

class Row {
public:
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
    Row() {
        id = 0;
        username[0] = '\0';
        email[0] = '\0';
    }
    Row(uint32_t id, const char* username, const char* email) {
        this->id = id;
        strncpy(this->username, username, COLUMN_USERNAME_SIZE+1);
        strncpy(this->email, email, COLUMN_EMAIL_SIZE+1);
    }
    bool operator< (const Row& other) {
        if (id < other.id) return true;
        if (id == other.id && strcmp(username, other.username) < 0) return true;
        if (id == other.id && strcmp(username, other.username) ==0 && strcmp(email, other.email) < 0) return true;
        else return false;
    }
    bool operator== (const Row& other) {
        if (id == other.id && strcmp(username, other.username) ==0 && strcmp(email, other.email)==0) return true;
        else return false;
    }
    bool operator<= (const Row& other) {
        return operator<(other) || operator==(other);
    }
    friend std::ostream& operator<< (std::ostream&, const Row&);
};

// 序列化和反序列化
/***
| 列       | 大小 ***(bytes)*** | 偏移量 ***(offset)*** |
| -------- | ------------------ | --------------------- |
| id       | 4                  | 0                     |
| username | 32                 | 4                     |
| email    | 255                | 36                    |
| 总计     | 291                |                       |

*/
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);

const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_SIZE + ID_OFFSET;
const uint32_t EMAIL_OFFSET = USERNAME_SIZE + USERNAME_OFFSET;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

void serialize_row(Row &row, void* destination);
void unserialize_row(Row &row, void* source);

/***
Table -> Page ->Row
Table -> pager -> page -> Row
*/

#define TABLE_MAX_PAGES 100
const uint32_t PAGES_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGES_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = TABLE_MAX_PAGES * ROWS_PER_PAGE;

/**
Pager
**/
class Pager {
public:
#if defined(_WIN32) || defined(_WIN64)
    FILE* file_descriptor;
#else
    int file_descriptor;
#endif
    uint32_t file_length;
    void* pages[TABLE_MAX_PAGES];

    Pager(const char* filename);
    void* get_page(uint32_t page_num);
    void pager_flush(uint32_t page_num, uint32_t size);
};

/***
Table
*/
class Table {
public:
    uint32_t num_rows;
    Pager pager;
    Table(const char* filename) : pager(filename) {
        num_rows = pager.file_length / ROW_SIZE;
    }

    ~Table();
    // read/write start with here, return void*
    void* row_slot(uint32_t row_num) {
        uint32_t page_num = row_num/ROWS_PER_PAGE; // 第几个page

        void* page = pager.get_page(page_num);
        // 计算这一页的偏移
        uint32_t row_offset = row_num % ROWS_PER_PAGE;
        uint32_t byte_offset = row_offset * ROW_SIZE;
        return (char *)page + byte_offset;
    }
};

#endif // TABLE_H_INCLUDED
