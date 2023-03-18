#pragma once
#ifndef SIMPLE_BTREE_H_INCLUDED
#define SIMPLE_BTREE_H_INCLUDED

#include <malloc.h>
#include <assert.h>
#include <queue>
#include <iostream>

typedef int KEY_VALUE;

typedef struct _BTREE_NODE {
    KEY_VALUE* keys;
    struct _BTREE_NODE** Childrens;
    int num; // key numbers
    bool leaf;
} BTREE_NODE, *PBTREE_NODE;

typedef struct _BTREE {
    BTREE_NODE* root;
    int t; // 2*t == 阶
}BTREE;

BTREE_NODE* btree_create_node(int t, bool leaf) {
    BTREE_NODE* node = (BTREE_NODE*)malloc(sizeof(BTREE_NODE));
    if (node == NULL) assert(0);
    node->leaf = leaf;
    node->keys = (KEY_VALUE*)calloc(1, (2 * t - 1) * sizeof(KEY_VALUE));
    node->Childrens = (BTREE_NODE**)calloc(1, (2 * t) * sizeof(BTREE_NODE*));
    node->num = 0;
    return node;
}
// create root node
void btree_create(BTREE* T, int t) {
    T->t = t;
    PBTREE_NODE x = btree_create_node(t, true);
    T->root = x;
}
// split node
void btree_split_child(BTREE* T, BTREE_NODE* x, int i) {
    // split node x index i
    int t = T->t;
    BTREE_NODE* y = x->Childrens[i]; // int node y split
    BTREE_NODE* z = btree_create_node(t, y->leaf);
    z->num = t - 1;
    /** |1|2|3|4|5|  --> t=3, index[0-1] = [3-4]  **/
    int j=0;
    for(j=0;j<t-1;j++) {
        z->keys[j] = y->keys[j+t];
    }

    if(!y->leaf) { // non-leaf node need to childrens
        for(j=0;j<t;j++) {
            z->Childrens[j] = y->Childrens[j+t];
        }
    }
    y->num = t-1; // 2*t-1 --> t-1,t-1, 1
    for (j=x->num;j>=i+1;j--){
        x->Childrens[j + 1] = x->Childrens[j];
    }
    x->Childrens[i + 1] = z;
    for(j=x->num -1;j>=i;j--) {
        x->keys[j+1] = x->keys[j]; // x的keys空出一个，接受y提升的key，例如3
    }
    x->keys[i] = y->keys[t-1];
    x->num += 1;
}

void btree_insert_notfull(BTREE* T, BTREE_NODE* x, KEY_VALUE k) {
    int i = x->num - 1; // get node number, -1 means index
    //std::cout<<"i="<<i<<std::endl;
    if (x->leaf == true) {
        while(i>=0 && x->keys[i] > k) {
            x->keys[i+1] = x->keys[i];
            i--;
        }
        x->keys[i+1]=k;
        x->num += 1;
    } else {
        // 找到叶子节点
        while(i>=0 && x->keys[i] >k) i--;

        if (x->Childrens[i+1]->num == 2*(T->t)-1) {
            btree_split_child(T, x, i+1);
            if(k > x->keys[i+1]) {
                i++;
            }
        }
        btree_insert_notfull(T, x->Childrens[i+1], k);
    }
}

void btree_insert(BTREE* T, KEY_VALUE key) {
    BTREE_NODE* r = T->root;

    if (r->num == 2*T->t - 1) {
        BTREE_NODE* node = btree_create_node(T->t, false);
        T->root = node;
        node->Childrens[0]=r;

        btree_split_child(T, node, 0);
        int i = 0;
        if (node->keys[0] < key) i++;
        btree_insert_notfull(T, node->Childrens[i], key);
    } else {
        btree_insert_notfull(T, r, key);
    }
}

void btree_print(BTREE* T) {
    BTREE_NODE* r = T->root;
    std::queue<BTREE_NODE*> que;
    que.push(r);
    while(!que.empty()) {
        int sz = que.size();
        while (sz--) {
            BTREE_NODE* p = que.front();
            que.pop();
            int x = p->num;
            for(int i=0;i<x+1;i++) {
                if(p->Childrens[i] != NULL) que.push(p->Childrens[i]);
            }
            for(int i=0;i<x;i++) {
                std::cout<<p->keys[i]<<" ";
            }
            std::cout<<"|| ";
        }
        std::cout<<std::endl;
    }

}
#endif // SIMPLE_BTREE_H_INCLUDED
