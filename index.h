//
// Created by 26685 on 2023/3/18.
//
#pragma once
#ifndef SIMPLE_SQL_INDEX_H
#define SIMPLE_SQL_INDEX_H

#include "btree/btree_template.h"
using namespace std;

struct Index {
    ::uint32_t unique_id;
    ::uint64_t pos;
    Index() = default;
    Index(::uint32_t _unique_id, ::uint64_t _pos) : unique_id(_unique_id), pos(_pos) { }
    explicit Index(::uint32_t _unique_id) :  unique_id(_unique_id), pos(0) { }
    Index(const Index& other) {
        unique_id = other.unique_id;
        pos = other.pos;
    }
    bool operator< (const Index& other) {
        return unique_id < other.unique_id;
    }
    bool operator== (const Index& other) {
        return unique_id == other.unique_id;
    }
    bool operator<= (const Index& other) {
        return operator<(other) || operator==(other);
    }
    friend std::ostream& operator<< (std::ostream&, const Index&);
};

class IndexTree {
public:
#if defined(_WIN32) || defined(_WIN64)
    ::FILE* file_descriptor;
#else
    int file_descriptor;
#endif
    Btree<Index> idxTree;
    ::uint32_t file_length;
    explicit IndexTree(const char* filename);
    ::uint64_t get_pos(Index& search_index);
    bool insert_index(Index& index);
    void traverse();
    void write_file();
};
#endif //SIMPLE_SQL_INDEX_H
