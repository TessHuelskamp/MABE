//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__WorldWeed__
#define __BasicMarkovBrainTemplate__WorldWeed__

#include "../AbstractWorld.h"

#include <stdlib.h>
#include <thread>
#include <vector>
#include <map>

using namespace std;

class WeedWorld : public AbstractWorld {

public:

    struct org{
        int x;
        int y;
        int ID; //TODO Add more stats
    };

	static shared_ptr<ParameterLink<int>> modePL;
	static shared_ptr<ParameterLink<int>> numberOfOutputsPL;
	static shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;

	int mode;
	int numberOfOutputs;
	int evaluationsPerGeneration;

	WeedWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~WeedWorld() = default;

	virtual void evaluate(map<string, shared_ptr<Group> > &groups, int analyse, int visualize, int debug);

	virtual int requiredInputs() override;
	virtual int requiredOutputs() override;

   // map<int, org>

/*    	// convert x,y into a grid index
	int getGridIndexFromXY(pair<int, int> loc) {
		return loc.first + (loc.second * WorldX);
	}

	// return value on grid at index
	int getGridValue(const vector<int> &grid, int index) {
		return grid[index];
	}

	// return the value on grid at x,y
	int getGridValue(const vector<int> &grid, pair<int, int> loc) {
		return getGridValue(grid, getGridIndexFromXY(loc));
	}

	// takes x,y and updates them by moving one step in facing
	pair<int, int> moveOnGrid(pair<int, int> loc, int facing) {
		return {loopMod((loc.first + xm[facing]), WorldX), loopMod((loc.second + ym[facing]), WorldY)};
	}

	// update value at index in grid
	void setGridValue(vector<int> &grid, int index, int value) {
		grid[index] = value;
	}

	// update value at x,y in grid
	void setGridValue(vector<int> &grid, pair<int, int> loc, int value) {
		setGridValue(grid, getGridIndexFromXY(loc), value);
	}

	// return a vector of size x*y
	vector<int> makeGrid(int x, int y) {
		vector<int> grid;
		grid.resize(x * y);
		return grid;
	}
*/


	//virtual int maxOrgsAllowed() override;
	//virtual int minOrgsAllowed() override;
};



#endif /* defined(__BasicMarkovBrainTemplate__WorldWeed__) */
