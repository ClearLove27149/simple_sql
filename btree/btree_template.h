//
// Created by 26685 on 2023/3/18.
//

#ifndef SIMPLE_SQL_BTREE_TEMPLATE_H
#define SIMPLE_SQL_BTREE_TEMPLATE_H
#include <iostream>
#include <list>
#include <deque>
using namespace std;


#define M 6
#define DEGREE (M/2)
//typedef int KEY_TYPE;
template<typename KEY_TYPE>
struct Btree_node
{
    std::list<KEY_TYPE> keys;
    std::list<Btree_node> childrens;
    int num;	//�ӽڵ����(Ӧ����key��Ŀ)
    bool isleaf = false;
    Btree_node()
    {
        keys.clear();
        childrens.clear();
        num = 0;
        isleaf = false;
    }
    bool operator< (const Btree_node& node)
    {
        return this->keys.back() < node.keys.front();
    }
    Btree_node* search(const KEY_TYPE& key)
    {
        if (num == 0)
            return nullptr;
        int i;
        typename std::list<KEY_TYPE>::iterator it = keys.begin();
        for (i = 0; i < num && *(it) < key; ++i, ++it);
        if (i < num && *(it) == key)
            return this;
        else if (isleaf)
            return nullptr;
        else
        {
            typename std::list<Btree_node>::iterator it_c = childrens.begin();
            for (int j = 0; j < i; ++it_c, ++j);
            return it_c->search(key);
        }

    }
    bool search2(KEY_TYPE& key) {
        if (num == 0)
            return false;
        int i;
        typename std::list<KEY_TYPE>::iterator it = keys.begin();
        for (i = 0; i < num && *(it) < key; ++i, ++it);
        if (i < num && *(it) == key) {
            key = *(it);
            return true;
        }
        else if (isleaf)
            return false;
        else
        {
            typename std::list<Btree_node>::iterator it_c = childrens.begin();
            for (int j = 0; j < i; ++it_c, ++j);
            return it_c->search2(key);
        }
    }
    void traverse();
};

template<typename KEY_TYPE>
class Btree
{
public:
    Btree_node<KEY_TYPE> root;
    Btree()
    {
        root.isleaf = true;
    }
    Btree_node<KEY_TYPE>* search(const KEY_TYPE& key)
    {
        return root.search(key);
    }
    bool search2(KEY_TYPE& key) {
        return root.search2(key);
    }
    bool B_insert(const KEY_TYPE& key);
    bool B_insert(const KEY_TYPE& key, Btree_node<KEY_TYPE>& node);  //key������
    bool B_split(Btree_node<KEY_TYPE>& parent, int i);//��һ��������Ҫ���ѵĽڵ�ĸ��ڵ㣬�ڶ��������Ƿ��ѽڵ�de���ڵ�ĵ�i���ӽڵ�
    bool B_split_root();//���ڵ����
    bool B_delete(const KEY_TYPE& key);
    bool B_delete(const KEY_TYPE& key, Btree_node<KEY_TYPE>& node);  //key����
    void traverse();
};

template<typename KEY_TYPE>
inline bool Btree<KEY_TYPE>::B_insert(const KEY_TYPE& key)
{
    //std::cout << "Btree insert: "<<key<<std::endl;
    Btree_node<KEY_TYPE>* ptr = search(key);
    if (ptr)
    {
        //��key�Ѵ���
        //cout << "��key�Ѵ���" << endl;
        return true;
    }
    else
    {
        return B_insert(key, root);
    }
}
template<typename KEY_TYPE>
inline bool Btree<KEY_TYPE>::B_insert(const KEY_TYPE& key, Btree_node<KEY_TYPE>& node)
{
    if (node.isleaf)//b���Ĳ�����Ҷ�ӽڵ�
    {
        if (node.num == M - 1) //�˽ڵ��Ǹ��ڵ�
        {
            B_split_root();
            return B_insert(key, root);
        }
        else
        {
            int i;
            typename std::list<KEY_TYPE>::iterator it = node.keys.begin();
            for (i = 0; i < node.num && *(it) <= key; ++i, ++it);
            node.keys.insert(it, key);
            ++node.num;
            return true;
        }

    }
    else
    {
        int i;
        typename std::list<KEY_TYPE>::iterator it = node.keys.begin();
        typename std::list<Btree_node<KEY_TYPE>>::iterator it_c = node.childrens.begin();
        for (i = 0; i < node.num && *(it) <= key; ++i, ++it, ++it_c);
        if ((it_c)->num == M - 1)
        {
            //����������£�������ڵ�keys�ﵽ���ֵ���򱾽ڵ��Ǹ��ڵ�
            if (node.num == M - 1)
            {
                B_split_root();
                return B_insert(key, root);
            }
            else
            {
                //��ִ��һ�η���
                B_split(node, i);
                return B_insert(key, node);
            }
        }
        else
        {
            return B_insert(key, *it_c);
        }
    }
}
template<typename KEY_TYPE>
inline bool Btree<KEY_TYPE>::B_split(Btree_node<KEY_TYPE>& parent, int i)
{
    typename std::list<KEY_TYPE>::iterator it = parent.keys.begin();
    typename std::list<Btree_node<KEY_TYPE>>::iterator it_c = parent.childrens.begin();
    for (int k = 0; k < i; ++it, ++it_c, ++k);
    Btree_node<KEY_TYPE> new_node;
    Btree_node<KEY_TYPE> node = *(it_c);
    new_node.isleaf = node.isleaf;
    for (int j = 0; j < DEGREE - 1; ++j)
    {
        new_node.keys.push_front(node.keys.back());
        ++new_node.num;
        node.keys.pop_back();
        --node.num;
        if (!new_node.isleaf)
        {
            new_node.childrens.push_front(node.childrens.back());
            node.childrens.pop_back();
        }
    }
    if (!new_node.isleaf)
    {
        new_node.childrens.push_front(node.childrens.back());
        node.childrens.pop_back();
    }

    parent.keys.insert(it, node.keys.back());
    ++parent.num;
    node.keys.pop_back();
    --node.num;
    *(it_c) = new_node;
    parent.childrens.insert(it_c, node);
    return true;
}
template<typename KEY_TYPE>
inline bool Btree<KEY_TYPE>::B_split_root()
{
    Btree_node<KEY_TYPE> node;
    node.childrens.push_front(root);
    B_split(node, 0);
    root = node;
    return true;
}
template<typename KEY_TYPE>
inline bool Btree<KEY_TYPE>::B_delete(const KEY_TYPE& key)
{
    Btree_node<KEY_TYPE>* ptr = search(key);
    if (ptr)
    {
        return B_delete(key, root);
    }
    else
    {
        //��key������
        return true;
    }
}
template<typename KEY_TYPE>
inline bool Btree<KEY_TYPE>::B_delete(const KEY_TYPE& key, Btree_node<KEY_TYPE>& node)
{
    int i;
    typename std::list<KEY_TYPE>::iterator it = node.keys.begin();
    for (i = 0; i < node.num && *(it) < key; ++i, ++it);
    if (i < node.num && *it == key)
    {
        if (node.isleaf)
        {
            node.keys.erase(it);
            --node.num;
            return true;
        }
        else
        {
            typename std::list<Btree_node<KEY_TYPE>>::iterator it_c = node.childrens.begin();
            for (int j = 0; j < i; ++j, ++it_c);
            //���ӽڵ��λ��ϲ�
            typename std::list<Btree_node<KEY_TYPE>>::iterator it_c_pre = it_c, it_c_post = ++it_c;
            if (it_c_pre->num > DEGREE - 1) //����������λ
            {
                KEY_TYPE temp = *it;
                *it = it_c_pre->keys.back();
                it_c_pre->keys.back() = temp;
                it_c = it_c_pre;
            }
            else if (it_c_post->num > DEGREE - 1)  //����������λ
            {
                KEY_TYPE temp = *it;
                *it = it_c_post->keys.front();
                it_c_post->keys.front() = temp;
                it_c = it_c_post;
            }
            else //�ϲ�
            {
                typename list<KEY_TYPE>::iterator kit;
                for (kit = it_c_pre->keys.begin(); kit != it_c_pre->keys.end() &&* kit < *it; ++kit);
                it_c_pre->keys.insert(kit, *it);
                ++(it_c_pre->num);
                it_c_pre->keys.merge(it_c_post->keys);
                it_c_pre->num += it_c_post->num;
                it_c_pre->childrens.merge(it_c_post->childrens);
                node.keys.erase(it);
                --node.num;
                node.childrens.erase(it_c_post);
                it_c = it_c_pre;

            }
            return B_delete(key, *it_c);
        }
    }
    else if(!node.isleaf)//key�ڸýڵ���ӽڵ�it_c
    {
        typename std::list<Btree_node<KEY_TYPE>>::iterator it_c = node.childrens.begin();
        for (int j = 0; j < i; ++j, ++it_c);
        if (it_c->num == DEGREE - 1)  //�ӽڵ�
        {
            typename std::list<Btree_node<KEY_TYPE>>::iterator it_c_pre = it_c, it_c_post = it_c;
            if (i > 0 && (--it_c_pre)->num > DEGREE - 1) //����ͬ����ǰ��һ��������num > DEGREE-1
            {
                it_c->keys.push_front(*(--it));
                ++(it_c->num);
                it_c->childrens.push_front(it_c_pre->childrens.back());
                *it = it_c_pre->keys.back();
                it_c_pre->keys.pop_back();
                --(it_c_pre->num);
                it_c_pre->childrens.pop_back();
            }
            else if (i < node.num && (++it_c_post)->num > DEGREE - 1) //����ͬ���ĺ���һ��������num > DEGREE-1
            {
                it_c->keys.push_back(*it);
                ++(it_c->num);
                it_c->childrens.push_back(it_c_post->childrens.front());
                *it = it_c_post->keys.front();
                it_c_post->keys.pop_front();
                --(it_c_post->num);
                it_c_post->childrens.pop_front();
            }
            else if (i > 0) //����ͬ����ǰ��һ��������num = DEGREE-1
            {
                if (node.num == 1) //node�Ǹ��ڵ�
                    root = *it_c_pre;
                it_c_pre->keys.push_back(*--it);
                ++(it_c_pre->num);
                it_c_pre->keys.merge(it_c->keys);
                it_c_pre->num += it_c->num;
                it_c_pre->childrens.merge(it_c->childrens);
                node.keys.erase(it);
                --node.num;
                node.childrens.erase(it_c);
                it_c = it_c_pre;
            }
            else if (i < node.num)//����ͬ���ĺ���һ��������num = DEGREE-1
            {
                if (node.num == 1) //node�Ǹ��ڵ�
                    root = *it_c;
                it_c->keys.push_back(*it);
                ++(it_c->num);
                it_c->keys.merge(it_c_post->keys);
                it_c->num += it_c_post->num;
                it_c->childrens.merge(it_c_post->childrens);
                node.keys.erase(it);
                --node.num;
                node.childrens.erase(it_c_post);

            }
            else //�д���
            {
            }
        }
        return B_delete(key, *it_c);
    }
    else //û�иýڵ�
    {
        return false;
    }

}
template<typename KEY_TYPE>
inline void Btree<KEY_TYPE>::traverse()
{
    cout << "---------------------------------------------------------------------------------------------------" << endl;
    if (root.num != 0) root.traverse();
}
template<typename KEY_TYPE>
inline void Btree_node<KEY_TYPE>::traverse()
{
    deque<Btree_node*> dq;
    int numOfdq = 0;
    int numOfNextdq = 0;
    dq.push_back(this);
    ++numOfdq;
    while (!dq.empty())
    {
        while (numOfdq--)
        {
            Btree_node* node = dq.front();
            cout << "[";
            for (typename list<KEY_TYPE>::const_iterator it = node->keys.cbegin(); it != node->keys.cend(); ++it)
            {
                cout << "|" << *it;
            }
            cout << "|";
            cout << "]  ";
            if (!node->isleaf)
            {
                for (typename list<Btree_node>::iterator it_c = node->childrens.begin(); it_c != node->childrens.end(); ++it_c)
                {
                    dq.push_back(&*it_c);
                    ++numOfNextdq;
                }
            }
            dq.pop_front();
        }
        cout << endl << "---------------------------------------------------------------------------------------------------" << endl;
        numOfdq = numOfNextdq;
        numOfNextdq = 0;
    }
}
#endif //SIMPLE_SQL_BTREE_TEMPLATE_H