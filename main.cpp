#include "db.h"


int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Must supply a database filename." << std::endl;
        exit(EXIT_FAILURE);
    }

    /*
    int file_descriptor = open(argv[1], O_CREAT);  // User read permission
    std::cout<<file_descriptor<<std::endl;

    close(file_descriptor);*/

    DB db(argv[1]);
    db.start();

    // test btree3
    //test simple_btree
//    BTREE* btree = (BTREE*)malloc(sizeof(BTREE));
//    btree_create(btree, 3);
//    for(int i=1;i<=50;i++) {
//        btree_insert(btree, i);
//    }
//    for(int i=100;i>=50;i--) {
//        btree_insert(btree, i);
//    }
//    btree_print(btree);

    return 0;
}
