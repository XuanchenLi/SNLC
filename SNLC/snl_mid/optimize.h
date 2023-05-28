#pragma once
#include"mid.h"
typedef struct twotuple {
	string name;
	int value;
}Twotuple;

class Optimizer {
	vector<Argnode>& table;
	vector< vector<Twotuple*>> blocks;
	unsigned int point;
public:
	Optimizer(vector<Argnode>& t) :table(t),point(0) {};

public:
	void ConstOptimize();
	bool DivBaseBlock();
	void OptiBlock();
	bool searchBlock(ArgRecord&);
	bool addToBlock(string name,int val);

};
