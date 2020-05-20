#include "index.h"
#include<iostream>
#include<vector>
#include<fstream>

using namespace std;

//=====class Node=====

Node::Node()
{
	is_leaf = false;
	size = 0;
	key = new int[Max];
	value = new int[Max];
	ptr = new Node*[Max+1];
	for (int i = 0; i < Max + 1; ++i)
		ptr[i] = NULL;
	front = NULL;
	back = NULL;
	parent = NULL;
}

//=====class BPtree=====

BPtree::BPtree()
{
	root = NULL;
}

void BPtree::insert(int key, int value)
{
	if (root == NULL)
	{
		root = new Node;
		root->is_leaf = true;
		root->key[0] = key;
		root->value[0] = value;
		root->size = 1;
	}
	else
	{
		Node* cur = root;
		while (cur->is_leaf == false)
		{
			for (int i = 0; i < cur->size; ++i)
			{
				if (key < cur->key[i])
				{
					cur = cur->ptr[i];
					break;
				}
				if (i == cur->size - 1)
				{
					cur = cur->ptr[i + 1];
					break;
				}
			}
		}
		if (cur->size < Max)
		{
			int insert_idx = 0;
			while (key >= cur->key[insert_idx] && insert_idx < cur->size)
				insert_idx++;
			for (int j = cur->size ; j > insert_idx; --j)
			{
				cur->key[j] = cur->key[j - 1];
				cur->value[j] = cur->value[j - 1];
			}
			cur->key[insert_idx] = key;
			cur->value[insert_idx] = value;
			cur->size++;
			// leaf node不用調整ptr，因為是空的
		}
		else
		{
			Node* newleaf = new Node;
			newleaf->is_leaf = true;
			newleaf->parent = cur->parent;
			//processing extra insert
			int virtualkey[Max + 1];
			int virtualvalue[Max + 1];
			for (int i = 0; i < Max; ++i)
			{
				virtualkey[i] = cur->key[i];
				virtualvalue[i] = cur->value[i];
			}
			int insert_idx = 0;
			while (key >= virtualkey[insert_idx] && insert_idx < Max)
				insert_idx++;
			for (int j = Max; j > insert_idx; --j)
			{
				virtualkey[j] = virtualkey[j - 1];
				virtualvalue[j] = virtualvalue[j - 1];
			}
			virtualkey[insert_idx] = key;
			virtualvalue[insert_idx] = value;
			// finish processing extra insert
			// it's time to split
			cur->size = (Max + 1) / 2;
			newleaf->size = (Max + 1) - cur->size;
			for (int i = 0; i < cur->size; ++i)
			{
				cur->key[i] = virtualkey[i];
				cur->value[i] = virtualvalue[i];
			}
			for (int i = 0, j = cur->size; i < newleaf->size; ++i, ++j)
			{
				newleaf->key[i] = virtualkey[j];
				newleaf->value[i] = virtualvalue[j];
			}
			//build "leaf" link list
			newleaf->back = cur->back;
			if (cur->back != NULL)
				cur->back->front = newleaf;
			cur->back = newleaf;
			newleaf->front = cur;
			//
			if (cur == root)//root is leaf
			{
				Node* newRoot = new Node;//internal Node
				newRoot->is_leaf = false;
				newRoot->key[0] = newleaf->key[0];
				newRoot->ptr[0] = cur;
				newRoot->ptr[1] = newleaf;
				newRoot->size = 1;
				//用parent指回去
				cur->parent = newRoot;
				newleaf->parent = newRoot;
				//
				root = newRoot;
			}
			else
			{
				insertInternal(newleaf->key[0], newleaf->parent, newleaf);
				//被推上去的key，leaf保留key
			}
		}
	}
}

void BPtree::insertInternal(int key, Node* cur, Node* child)
{
	if (cur->size < Max)
	{
		int insert_idx = 0;
		while (key >= cur->key[insert_idx] && insert_idx < cur->size)
			insert_idx++;
		for (int j = cur->size; j > insert_idx; --j)
		{
			cur->key[j] = cur->key[j - 1];
		}
		for (int j = cur->size + 1; j > insert_idx + 1; --j)
		{
			cur->ptr[j] = cur->ptr[j - 1];
		}
		cur->key[insert_idx] = key;
		cur->ptr[insert_idx + 1] = child;
		cur->size++;
	}
	else//size太大，就要分裂
	{
		Node* newinternal = new Node;
		newinternal->is_leaf = false;
		newinternal->parent = cur->parent;
		//processing extra insert
		int virtualkey[Max + 1];
		Node* virtualptr[Max + 2];
		for (int i = 0; i < Max; ++i)
		{
			virtualkey[i] = cur->key[i];
		}
		for (int i = 0; i < Max + 1; ++i)
		{
			virtualptr[i] = cur->ptr[i];
		}
		int insert_idx = 0;
		while (key >= virtualkey[insert_idx] && insert_idx < Max)
			insert_idx++;
		for (int j = Max; j > insert_idx; --j)
		{
			virtualkey[j] = virtualkey[j - 1];
		}
		virtualkey[insert_idx] = key;

		for (int j = Max + 1; j > insert_idx; --j)
		{
			virtualptr[j] = virtualptr[j - 1];
		}
		virtualptr[insert_idx + 1] = child;
		//it's time to split
		cur->size = (Max + 1) / 2;
		newinternal->size = Max - cur->size;
		//一半留給cur
		for (int i = 0; i < cur->size; ++i)
		{
			cur->key[i] = virtualkey[i];
		}
		for (int i = 0; i < cur->size + 1; ++i)
		{
			cur->ptr[i] = virtualptr[i];
			virtualptr[i]->parent = cur;//parent也要重新指派
		}
		//一半newinteral帶走(注意!最中間的key要推上去，所以從cur->size "+ 1"開始)
		for (int i = 0, j = cur->size + 1; i < newinternal->size; ++i, ++j)
		{
			newinternal->key[i] = virtualkey[j];
		}
		for (int i = 0, j = cur->size + 1; i < newinternal->size + 1; ++i, ++j)
		{
			newinternal->ptr[i] = virtualptr[j];
			virtualptr[j]->parent = newinternal;//parent也要重新指派
		}
		//
		if (cur == root)
		{
			Node* newRoot = new Node;
			newRoot->is_leaf = false;
			newRoot->key[0] = virtualkey[(Max + 1) / 2];
			newRoot->ptr[0] = cur;
			newRoot->ptr[1] = newinternal;
			newRoot->size = 1;
			//
			cur->parent = newRoot;
			newinternal->parent = newRoot;
			//
			root = newRoot;
		}
		else
		{
			insertInternal(virtualkey[(Max + 1)/2], newinternal->parent, newinternal);
						  //最中間的key"不保留"推上去
		}
	}
}

int BPtree::search(int key)
{
	if (root == NULL)
		return -1;

	Node* cur = root;
	while (cur->is_leaf == false)
	{
		for (int i = 0; i < cur->size; ++i)
		{
			if (key < cur->key[i])
			{
				cur = cur->ptr[i];
				break;
			}
			if (i == cur->size - 1)
			{
				cur = cur->ptr[i + 1];
				break;
			}
		}
	}
	for (int i = 0; i < cur->size; ++i)
	{
		if (key == cur->key[i])
		{
			//found
			return cur->value[i];
		}
	}
	//Not found
	return -1;
}

int BPtree::range_query(int key1, int key2)
{
	int max = -1;
	int start = 0;
	if (root == NULL)
		return max;
	Node* cur = root;
	while (cur->is_leaf == false)
	{
		for (int i = 0; i < cur->size; ++i)
		{
			if (key1 < cur->key[i])
			{
				cur = cur->ptr[i];
				break;
			}
			if (i == cur->size - 1)
			{
				cur = cur->ptr[i + 1];
				break;
			}
		}
	}
	//開找
	for (int i = 0; i < cur->size; ++i)
	{
		if (key1 < cur->key[0])
		{
			start = 0;
			break;
		}
		if (cur->key[i] >= key1)
		{
			start = i;
			break;
		}
		if (key1 > cur->key[cur->size - 1])
		{
			cur = cur->back;
			start = 0;
			break;
		}
	}
	int idx = 0;
	for (idx = start; idx < cur->size && cur->key[idx] <= key2 ; ++idx)
	{
		if (cur->value[idx] > max)
			max = cur->value[idx];
	}
	cur = cur->back;
	while (true)
	{
		if (cur == NULL)
			return max;
		if (cur->key[0] > key2)
			return max;
		for (idx = 0; idx < cur->size && cur->key[idx] <= key2; ++idx)
		{
			if (cur->value[idx] > max)
				max = cur->value[idx];
		}
		cur = cur->back;
	}
}

//=====class Index=====

Index::Index(int num_rows, vector<int>& key, vector<int>& value)
{
	BPtree* BP = new BPtree;
	Tree = BP;
	for (int i = 0; i < num_rows; ++i)
	{
		BP->insert(key[i], value[i]);
	}
}

void Index::key_query(vector<int>& query_keys)
{
	ofstream file;
	file.open("key_query_out.txt");
	for (int i = 0; i < query_keys.size(); ++i)
		file << Tree->search(query_keys[i]) << endl;
	file.close();
}

void Index::range_query(vector<pair<int, int>>& query_pairs)
{
	ofstream file;
	file.open("range_query_out.txt");
	for (int i = 0; i < query_pairs.size(); ++i)
		file << Tree->range_query(query_pairs[i].first, query_pairs[i].second) << endl;
	file.close();
}

void Index::clear_the_node(Node* cur)
{
	if (cur->is_leaf == false)
	{
		for (int i = 0; i < cur->size; ++i)
		{
			clear_the_node(cur->ptr[i]);
		}
	}
	delete[] cur->ptr;
	delete[] cur->key;
	delete[] cur->value;
	delete cur;
}

void Index::clear_index()
{
	Node* cur = Tree->getroot();
	clear_the_node(cur);
	delete Tree;
}

//test memory leak valgrind --tool=memcheck program_name