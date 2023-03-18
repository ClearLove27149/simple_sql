#ifndef BTREE2_H_INCLUDED
#define BTREE2_H_INCLUDED

#include <iostream>
#include <list>
#include <vector>
#include <math.h>
#include <queue>

template<typename K, typename V>
struct Pair_KV {
    K key;
    V value;
    bool operator< (const Pair_KV& _temp) {
        return key < _temp.key;
    }
    Pair_KV(K key, V value) {
        this->key = key;
        this->value= value;
    }
};

template<typename K, typename V>
struct Node {
public:
    int size; // 当前节点包含的健数量
    Node *parent;
    std::vector<Pair_KV<K, V>> items;
    //std::priority_queue<std::pair<K, V>, std::vector<std::pair<K,V>>, [](std::pair<K,V> A, std::pair<K,V> B) { return A.first < B.first;}> items;
    std::vector<Node<K, V>*> children;
    //std::priority_queue<Node*, std::vector<Node*>, [] (Node* A, Node* B) {
     //   A->items.top() < B->items.top();}> childrens;
    //std::list<Node*> childrens; // 子节点数组，根据健升序
    bool isLeaf = false;
    bool operator< (Node<K, V> &temp) {
        return this->items.back() < temp.items.front();
    }
    Node(int _size, K key, V value) {
        this->size = _size;
        items.emplace_back(new Pair_KV(key, value));
    }
    //K get_key()
};

template<typename K, typename V>
struct Btree {
public:
    Node<K, V>* root;
    //
    V get(K key) {
        if (root == nullptr) {
            return nullptr;
        }
        //// 1.先根据 Key 找到包含该 Key 的节点
        std::pair<int, Node<K,V>*> res = search(key);
        Pair_KV<K, V> item = res.second->items[res.first];

        if (item != nullptr && item.key == key) {
            return item.value;
        }
        return nullptr;
    }
    /**
    查找键所在的节点 及 键在该节点的索引位置
    **/
    std::pair<int, Node<K, V>*> search(K key) {
        Node<K,V> *p = root;
        while(true) {
            // binary search
            int m = p->size / 2;
            int upper = p->size;
            int lower = 0;
            while (m >= lower && m < upper) {
                K key_temp = p->items[m].key;
                if (key < key_temp) {
                    upper = m;
                    m = m - round((float) (m - lower) / 2);
                } else if (key > key_temp) {
                    lower = m;
                    m = m + round((float) (upper - m) / 2);
                } else {
                    return std::make_pair(m, p);
                }

            }
            // 1.2.节点不包含该键
            // 1.2.1.已到达叶子节点，结束查找
            if (p->isLeaf) {
                return std::make_pair(m, p);
            }
            // 1.2.2 没达到叶子节点
            p = p->childrens[m];
        }
    }
    // insert
    void put(K _key, V _value) {
        // 1.如果为空树，创建根节点并添加键值对
        if (root == nullptr) {
            root = new Node<K, V>(0, _key, _value);
            //root = new Node<>(maxOrder, Pairs.of(key, value));
            //size.increment();
            //height.increment();
            return;
        }
        std::pair<int, Node<K,V>*> res = search(key);
        Pair_KV<K, V> item = res.second->items[res.first];

        if (item != nullptr && item.key == key) {
            res.second->items[res.first].value = _value;
            return;
        }

    }
};


#endif // BTREE2_H_INCLUDED
