#pragma once
#include <vector>
#include <queue>
#include "PCB.h"
#include "Memory.h"
#include <iostream>
using namespace std;

class Swapper
{
private:
	PCB* beingSwapped;
	vector<PCB*> LTS;
	queue<PCB*> swapOutQ;
	bool swappingIn;
	bool swappingOut;
public:
	Swapper();
	void runFromLTS(int &, int[], Memory&);
	bool swapIn(int &, int[], PCB *, int);
	void swapOut(int &, int[], Memory&);
	void swapFromLTS(int &, int[], Memory&);
	bool isSwappingIn();
	bool isSwappingOut();
	void setSwappingIn(bool);
	void setSwappingOut(bool);
	PCB* getJobBeingSwapped();
	void addToLTS(PCB*);
};

