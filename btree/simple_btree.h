#pragma once
#ifndef SIMPLE_BTREE2_H_INCLUDED
#define SIMPLE_BTREE2_H_INCLUDED

#include <iostream>
#include <malloc.h>
#include <assert.h>
#include <queue>

#define DEGREE		3
typedef int KEY_VALUE;

typedef struct _BTREE_NODE
{
    KEY_VALUE* keys;
    struct _BTREE_NODE** Childrens;
    int num;
    int leaf;
}BTREE_NODE,*PBTREE_NODE;

typedef struct _BTREE
{
    BTREE_NODE* root;
    int t;
}BTREE;

BTREE_NODE* btree_create_node(int t, int leaf) {

    BTREE_NODE* node = (BTREE_NODE*)calloc(1, sizeof(BTREE_NODE));
    if (node == NULL) assert(0);

    node->leaf = leaf;
    node->keys = (KEY_VALUE*)calloc(1, (2 * t - 1) * sizeof(KEY_VALUE));
    node->Childrens = (BTREE_NODE**)calloc(1, (2 * t) * sizeof(BTREE_NODE*));
    node->num = 0;

    return node;
}
//�ڵ����
void btree_split_child(BTREE* T,BTREE_NODE* x,int i)
{
    int t = T->t;

    BTREE_NODE* y = x->Childrens[i];
    BTREE_NODE* z = btree_create_node(t, y->leaf);
    z->num = t - 1;

    int j = 0;
    for (j=0;j<t-1;j++)
    {
        z->keys[j] = y->keys[j + t];
    }
    if (y->leaf==0)
    {
	    for (j=0;j<t;j++)
	    {
            z->Childrens[j] = y->Childrens[j + t];
	    }
    }
    y->num = t - 1;
    for (j=x->num;j>=i+1;j--)
    {
        x->Childrens[j + 1] = x->Childrens[j];
    }
    x->Childrens[i + 1] = z;
    for(j=x->num-1;j>=i;j--)
    {
        x->keys[j + 1] = x->keys[j];
    }
    x->keys[i] = y->keys[t - 1];
    x->num += 1;
}
//�����ڵ�
void btree_create(BTREE*T,int t)
{
    T->t = t;
    PBTREE_NODE x = btree_create_node(t, 1);
    T->root = x;
}
void btree_insert_notfull(BTREE*T,BTREE_NODE *x,KEY_VALUE k)
{
	//��ȡ�ڵ�����,��0��ʼ��1
    int i = x->num-1;
    //ֻ��1��Ҷ�ӽڵ�
	if (x->leaf==1)
	{
		while (i>=0&&x->keys[i]>k)
		{
            x->keys[i + 1] = x->keys[i];
            i--;
		}
        //��ֵ
        x->keys[i + 1] = k;
        x->num += 1;
	}else
	{
       //�ҵ�Ӧ�ò����Ҷ�ӽڵ�
        while (i >= 0 && x->keys[i] > k) i--;
        //�Ƿ��Ѿ�����5���ڵ�
       if (x->Childrens[i+1]->num==((2*T->t))-1)
       {
           btree_split_child(T, x, i + 1);
           if (k>x->keys[i+1])
           {
               i++;
           }
       }

       btree_insert_notfull(T, x->Childrens[i + 1], k);


	}
}
void btree_insert(BTREE *T ,KEY_VALUE key)
{
   //��ȡͷ�ڵ�
    BTREE_NODE* r = T->root;
    //������ڵ��Ҫ��������Ĳ���

    if (r->num==2*T->t-1)
    {
        BTREE_NODE* node = btree_create_node(T->t, 0);
        T->root = node;

        node->Childrens[0] = r;

        btree_split_child(T, node, 0);

        int i = 0;
        if (node->keys[0] < key) i++;
        btree_insert_notfull(T, node->Childrens[i], key);
    }
    else
    {
	    //���û������Ҫ��������Ĳ���
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

//�ͷŽڵ�
void btree_destory_node(BTREE_NODE* node)
{
	if (node == nullptr)
	{
		return;
	}
	free(node->Childrens);
	free(node->keys);
	free(node);
}
void btree_merge(BTREE* T, BTREE_NODE* node, int idx)
{
	BTREE_NODE* left = node->Childrens[idx];
	BTREE_NODE* right = node->Childrens[idx + 1];

	int i = 0;
	left->keys[T->t - 1] = node->keys[idx];
	//��ʼ���ݵĺϲ�
	for (i = 0; i < T->t - 1; i++)
	{
		left->keys[T->t + 1] = right->keys[i];
	}
	if (!left->leaf)
	{
		for (i = 0; i < T->t; i++)
		{
			left->Childrens[T->t + 1] = right->Childrens[i];
		}
	}
	left->num += T->t;
	//�ϲ���ɴݻٽڵ�
	btree_destory_node(right);

	//node
	for (i = idx + 1; i < node->num; i++)
	{
		node->keys[i - 1] = node->keys[i];
		node->Childrens[i] = node->Childrens[i + 1];
	}
	node->Childrens[i + 1] = NULL;
	node->num -= 1;
	if (node->num == 0)
	{
		T->root = left;
		btree_destory_node(node);
	}

}
void btree_delete_key(BTREE* T, BTREE_NODE* node, KEY_VALUE key)
{
	//����ǿսڵ㣬ֱ�ӷ���
	if (node == nullptr)
	{
		return;
	}
	int idx = 0, i;
	//��ȡkey���ڵ�λ��
	while (idx<node->num && key>node->keys[idx])
	{
		idx++;
	}
	if (idx < node->num && key == node->keys[idx])
	{
		if (node->leaf)
		{
			//�����Ҷ�ӽڵ㣬ֱ��ɾ��
			for (i = idx; i < node->num - 1; i++)
			{
				node->keys[i] = node->keys[i + 1];
			}
			node->keys[node->num - 1] = 0;
			node->num--;
			//����Ǹ��ڵ�����
			if (node->num == 0)
			{
				free(node);
				T->root = nullptr;
			}
			return;
		}//ֱ��ɾ��
		else if (node->Childrens[idx]->num >= T->t)
		{
			BTREE_NODE* left = node->Childrens[idx];
			node->keys[idx] = left->keys[left->num - 1];
			btree_delete_key(T, left, left->keys[left->num - 1]);
		}//ֱ��ɾ��
		else if (node->Childrens[idx + 1]->num >= T->t)
		{
			BTREE_NODE* right = node->Childrens[idx + 1];
			node->keys[idx] = right->keys[0];
			btree_delete_key(T, right, right->keys[0]);

		}
		else {
			//���������,˵�������Һ��ӽڵ㶼��T-1���ؼ���
			btree_merge(T, node, idx);
			btree_delete_key(T, node->Childrens[idx], key);
		}
	}
	else
	{
		BTREE_NODE* child = node->Childrens[idx];
		if (child == NULL)
		{
			printf("Can\'t del key=%d\n", key);
			return;
		}//�ӽڵ����Ŀ�պõ���2
		if (child->num == T->t - 1)
		{
			BTREE_NODE* left = nullptr;
			BTREE_NODE* right = nullptr;
			if (idx - 1 >= 0)
			{
				left = node->Childrens[idx - 1];
			}
			if (idx + 1 <= node->num)
			{
				right = node->Childrens[idx + 1];
			}
			//������ҽڵ��κ�һ�������Խ��ýڵ�
			if ((left && left->num >= T->t) || (right && right->num >= T->t))
			{
				int richR = 0;
				if (right)
				{
					richR = 1;
				}
				if (left && right)
				{
					richR = (right->num > left->num) ? 1 : 0;
				}
				//���ҽ��ýڵ�
				if (right && right->num >= T->t && richR)
				{
					child->keys[child->num] = node->keys[idx];
					child->Childrens[child->num + 1] = right->Childrens[0];
					child->num++;
					node->keys[idx] = right->keys[0];
					//�����ұߵĽڵ�
					for (i = 0; i < right->num - 1; i++)
					{
						right->keys[i] = right->keys[i + 1];
						right->Childrens[i] = right->Childrens[i + 1];
					}

					right->keys[right->num - 1] = 0;
					right->Childrens[right->num - 1] = right->Childrens[right->num];
					right->Childrens[right->num] = NULL;
					right->num--;
				}
				else
				{
					//�����ڵ�
					for (i = child->num; i > 0; i--)
					{
						child->keys[i] = child->keys[i - 1];
						child->Childrens[i + 1] = child->Childrens[i];
					}
					child->Childrens[1] = child->Childrens[0];
					child->Childrens[0] = left->Childrens[left->num];
					child->keys[0] = node->keys[idx - 1];
					child->num++;

					node->keys[idx - 1] = left->keys[left->num - 1];
					left->keys[left->num - 1] = 0;
					left->Childrens[left->num] = NULL;
					left->num--;
				}


			}
			else if ((!left) || (left->num == T->t - 1) && (!right) || (right->num == T->t - 1))
			{
				if (left&&left->num==T->t-1)
				{
					btree_merge(T, node, idx - 1);
					child = left;
				}else if(right&&right->num==T->t-1)
				{
					btree_merge(T, node, idx);
				}
			}
			btree_delete_key(T, child, key);
		}
	}

}
int btree_delete(BTREE* T, KEY_VALUE key)
{
	if (!T->root)
	{
		return -1;
	}
	btree_delete_key(T, T->root, key);
	return 0;
}


#endif // SIMPLE_BTREE2_H_INCLUDED