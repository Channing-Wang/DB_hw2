#ifndef INDEX_H
#define INDEX_H

#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<vector>

#define Max 100 // order = 50

using namespace std;

class Node
{
private:
	bool is_leaf;
	int size;
	int* key;
	int* value;
	Node* front;
	Node* back;
	Node* parent;
	Node** ptr;
public:
	Node();
	~Node();
	friend class BPtree;
};

class BPtree
{
private:
	Node* root;

public:
	BPtree();
	void insert(int key, int value);
	void insertInternal(int key, Node*, Node*);
	int search(int key);
	int range_query(int, int);
	//
	void print(Node*);
	Node* getroot() { return root; };
};

class Index
{
public:
	Index(int, vector<int>&, vector<int>&);
	~Index();
	void key_query(vector<int>&);
	void range_query(vector<pair<int, int>>&);
	void clear_index();

	friend class BPtree;
	BPtree* Tree;
};

#endif
