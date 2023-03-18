//
// Created by 26685 on 2023/3/18.
//
#include "index.h"

//Index
std::ostream& operator<< (std::ostream& out, const Index& index) {
    char buf[1024] = {0,};
    ::sprintf(buf, "{unique_id=%d,pos=%llx}", index.unique_id, index.pos);
    out<<std::string(buf);
    return out;
}

// indexfile
IndexTree::IndexTree(const char *filename) {
#if defined(_WIN32) || defined(_WIN64)
    file_descriptor = ::fopen(filename, "a+");
    if (file_descriptor == NULL) {
        printf_s("Error: cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    ::fseek(file_descriptor, 0, SEEK_END);
    file_length = ::ftell(file_descriptor);
#else
    file_descriptor = open(filename, O_RDWR | O_CREAT | 0x00400 | 0x00200);
    if (file_descriptor < 0) {
        std::cerr << "Error: cannot open file "<<filename<<std::endl;
        exit(EXIT_FAILURE);
    }
    file_length = lseek(file_descriptor, 0, SEEK_END);
#endif
}

::uint64_t IndexTree::get_pos(Index &search_index) {
    if (idxTree.search2(search_index)) {
        return search_index.pos;
    }
    return 0;
}

bool IndexTree::insert_index(Index &index) {
    //std::cout<<"IndexTree insert pos: " <<index<<std::endl;
    return idxTree.B_insert(index);
}

void IndexTree::traverse() {
    idxTree.traverse();
}

void IndexTree::write_file() {
    //todo
}
