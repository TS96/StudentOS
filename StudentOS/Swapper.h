/*
Tarek Saidee
CISC 3320
*/

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
	PCB* beingSwapped;			//stores the job currently being swapped
	priority_queue<PCB*, vector<PCB*>, PCB> LTS;			//Long term scheduler priority queue with a custom sort function
	vector<PCB*> swapOutQ;		//Vector of jobs to be swapped out
	bool swappingIn;			//Is there a job being swapped in?
	bool swappingOut;			//Is there a job being swapped out?
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
	void addToSwapOutQ(PCB*);
	bool swapOutQEmpty();
};

