#include <iostream>
using namespace std;

#include "btree_template.h"

template<typename KEY_TYPE>
void test01(Btree<KEY_TYPE> &mytree)
{

	for (int i = 0; i < 26; ++i)
	{
		mytree.B_insert(i);
		mytree.traverse();
	}
}
template<typename KEY_TYPE>
void test02(Btree<KEY_TYPE> &mytree)
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
template<typename KEY_TYPE>
void test03(Btree<KEY_TYPE>& mytree)
{
	test01(mytree);
	mytree.B_delete(8);
	mytree.traverse();

	mytree.B_delete(20);
	mytree.traverse();
}
int test()
{

	Btree<int> mytree;
	test03(mytree);


	return 0;
}
