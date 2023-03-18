#ifndef BTREE_H_INCLUDED
#define BTREE_H_INCLUDED

#include <iostream>
#include <list>
#include <deque>
using namespace std;


#define M 6
#define DEGREE (M/2)
typedef int KEY_TYPE;

struct Btree_node
{
	std::list<KEY_TYPE> keys;
	std::list<Btree_node> childrens;
	int num;	//子节点个数(应该是key数目)
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
		std::list<KEY_TYPE>::iterator it = keys.begin();
		for (i = 0; i < num && *(it) < key; ++i, ++it);
		if (i < num && *(it) == key)
			return this;
		else if (isleaf)
			return nullptr;
		else
		{
			std::list<Btree_node>::iterator it_c = childrens.begin();
			for (int j = 0; j < i; ++it_c, ++j);
			return it_c->search(key);
		}

	}
	void traverse();
};
class Btree
{
public:
	Btree_node root;
	Btree()
	{
		root.isleaf = true;
	}
	Btree_node* search(const KEY_TYPE& key)
	{
		return root.search(key);
	}
	bool B_insert(const KEY_TYPE& key);
	bool B_insert(const KEY_TYPE& key, Btree_node& node);  //key不存在
	bool B_split(Btree_node& parent, int i);//第一个参数是要分裂的节点的父节点，第二个参数是分裂节点de父节点的第i个子节点
	bool B_split_root();//根节点分裂
	bool B_delete(const KEY_TYPE& key);
	bool B_delete(const KEY_TYPE& key, Btree_node& node);  //key存在
	void traverse();
};


inline bool Btree::B_insert(const KEY_TYPE& key)
{
	Btree_node* ptr = search(key);
	if (ptr)
	{
		//此key已存在
		//cout << "此key已存在" << endl;
		return true;
	}
	else
	{
		return B_insert(key, root);
	}
}
inline bool Btree::B_insert(const KEY_TYPE& key, Btree_node& node)
{
	if (node.isleaf)//b树的插入在叶子节点
	{
		if (node.num == M - 1) //此节点是根节点
		{
			B_split_root();
			return B_insert(key, root);
		}
		else
		{
			int i;
			std::list<KEY_TYPE>::iterator it = node.keys.begin();
			for (i = 0; i < node.num && *(it) <= key; ++i, ++it);
			node.keys.insert(it, key);
			++node.num;
			return true;
		}

	}
	else
	{
		int i;
		std::list<KEY_TYPE>::iterator it = node.keys.begin();
		std::list<Btree_node>::iterator it_c = node.childrens.begin();
		for (i = 0; i < node.num && *(it) <= key; ++i, ++it, ++it_c);
		if ((it_c)->num == M - 1)
		{
			//在这种情况下，如果本节点keys达到最大值，则本节点是根节点
			if (node.num == M - 1)
			{
				B_split_root();
				return B_insert(key, root);
			}
			else
			{
				//先执行一次分裂
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
inline bool Btree::B_split(Btree_node& parent, int i)
{
	std::list<KEY_TYPE>::iterator it = parent.keys.begin();
	std::list<Btree_node>::iterator it_c = parent.childrens.begin();
	for (int k = 0; k < i; ++it, ++it_c, ++k);
	Btree_node new_node;
	Btree_node node = *(it_c);
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
inline bool Btree::B_split_root()
{
	Btree_node node;
	node.childrens.push_front(root);
	B_split(node, 0);
	root = node;
	return true;
}

inline bool Btree::B_delete(const KEY_TYPE& key)
{
	Btree_node* ptr = search(key);
	if (ptr)
	{
		return B_delete(key, root);
	}
	else
	{
		//此key不存在
		return true;
	}
}
inline bool Btree::B_delete(const KEY_TYPE& key, Btree_node& node)
{
	int i;
	std::list<KEY_TYPE>::iterator it = node.keys.begin();
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
			std::list<Btree_node>::iterator it_c = node.childrens.begin();
			for (int j = 0; j < i; ++j, ++it_c);
			//向子节点借位或合并
			std::list<Btree_node>::iterator it_c_pre = it_c, it_c_post = ++it_c;
			if (it_c_pre->num > DEGREE - 1) //向左子树借位
			{
				KEY_TYPE temp = *it;
				*it = it_c_pre->keys.back();
				it_c_pre->keys.back() = temp;
				it_c = it_c_pre;
			}
			else if (it_c_post->num > DEGREE - 1)  //向右子树借位
			{
				KEY_TYPE temp = *it;
				*it = it_c_post->keys.front();
				it_c_post->keys.front() = temp;
				it_c = it_c_post;
			}
			else //合并
			{
				list<KEY_TYPE>::iterator kit;
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
	else if(!node.isleaf)//key在该节点的子节点it_c
	{
		std::list<Btree_node>::iterator it_c = node.childrens.begin();
		for (int j = 0; j < i; ++j, ++it_c);
		if (it_c->num == DEGREE - 1)  //子节点
		{
			std::list<Btree_node>::iterator it_c_pre = it_c, it_c_post = it_c;
			if (i > 0 && (--it_c_pre)->num > DEGREE - 1) //存在同级的前面一颗树，且num > DEGREE-1
			{
				it_c->keys.push_front(*(--it));
				++(it_c->num);
				it_c->childrens.push_front(it_c_pre->childrens.back());
				*it = it_c_pre->keys.back();
				it_c_pre->keys.pop_back();
				--(it_c_pre->num);
				it_c_pre->childrens.pop_back();
			}
			else if (i < node.num && (++it_c_post)->num > DEGREE - 1) //存在同级的后面一颗树，且num > DEGREE-1
			{
				it_c->keys.push_back(*it);
				++(it_c->num);
				it_c->childrens.push_back(it_c_post->childrens.front());
				*it = it_c_post->keys.front();
				it_c_post->keys.pop_front();
				--(it_c_post->num);
				it_c_post->childrens.pop_front();
			}
			else if (i > 0) //存在同级的前面一颗树，且num = DEGREE-1
			{
				if (node.num == 1) //node是根节点
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
			else if (i < node.num)//存在同级的后面一颗树，且num = DEGREE-1
			{
				if (node.num == 1) //node是根节点
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
			else //有错误
			{
			}
		}
		return B_delete(key, *it_c);
	}
	else //没有该节点
	{
		return false;
	}

}
inline void Btree::traverse()
{
	cout << "---------------------------------------------------------------------------------------------------" << endl;
	if (root.num != 0) root.traverse();
}
inline void Btree_node::traverse()
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
			for (list<KEY_TYPE>::const_iterator it = node->keys.cbegin(); it != node->keys.cend(); ++it)
			{
				cout << "|" << *it;
			}
			cout << "|";
			cout << "]  ";
			if (!node->isleaf)
			{
				for (list<Btree_node>::iterator it_c = node->childrens.begin(); it_c != node->childrens.end(); ++it_c)
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

//test
void test01(Btree &mytree)
{

	for (int i = 0; i < 26; ++i)
	{
		mytree.B_insert(i);
		mytree.traverse();
	}
}
void test02(Btree &mytree)
{

	for (int i = 0; i < 4; ++i)
	{
		mytree.B_insert(i);
		mytree.traverse();
	}
	for (int i = 12; i < 19; ++i)
	{
		mytree.B_insert(i);
		mytree.traverse();
	}
	for (int i = 25; i > 20; --i)
	{
		mytree.B_insert(i);
		mytree.traverse();
	}
	for(int i = 5; i < 12; ++i)
	{
		mytree.B_insert(i);
		mytree.traverse();
	}
	mytree.B_insert(4);
	mytree.traverse();
	mytree.B_insert(19);
	mytree.traverse();
	mytree.B_insert(20);
	mytree.traverse();
	mytree.B_insert(1);
	mytree.traverse();

}
void test03(Btree& mytree)
{
	test01(mytree);
	mytree.B_delete(8);
	mytree.traverse();

	mytree.B_delete(20);
	mytree.traverse();
}
int test()
{

	Btree mytree;
	test03(mytree);


	return 0;
}

#endif // BTREE_H_INCLUDED
