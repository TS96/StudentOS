#ifndef Memory_H
#define Memory_H
#include "PCB.h"
#include <vector>
#include <queue>
using namespace std;
class Memory
{
private:
	vector<pair<int,int> > FST;
	int findSpot(int);
	queue<PCB> jobs;
	PCB jobDoingIO;
	bool beKilled;
public:
	Memory();
	bool insertNewJob(PCB &);
	void mergeAdjacentSpaces();
	bool deleteFromMemory(PCB &);
	static bool sortBySize(pair<int, int>, pair<int, int>);
	static bool sortByAddress(pair<int, int>, pair<int, int>);
	void printFST();
	PCB& getNextJob();
	bool isEmpty();
	void pop();
	void push(PCB);
	int getCount();
	int findMemPos(PCB);
	void blockJob();
	void killAfterIO(PCB);
	void setJobDoingIO(PCB);
	PCB& getJobDoingIO();
	bool shouldKill();
};

#endif