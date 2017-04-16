#ifndef Memory_H
#define Memory_H
#include "PCB.h"
#include <vector>
using namespace std;
class Memory
{
private:
	vector<pair<int,int> > FST;
	int findSpot(int);
public:
	Memory();
	bool insertNewJob(PCB &);
	void mergeAdjacentSpaces();
	bool deleteFromMemory(PCB &);
	static bool sortBySize(pair<int, int>, pair<int, int>);
	static bool sortByAddress(pair<int, int>, pair<int, int>);
	void printFST();
};

#endif