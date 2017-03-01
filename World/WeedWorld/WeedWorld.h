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


//cliff you can yell at us later for this :) 
#include <iostream> 
#include <stdlib.h>
#include <thread>
#include <vector>
#include <map>


//...
using namespace std;

class WeedWorld : public AbstractWorld {

public:

    struct org{
        int orgID; 
        int loc;
        int numFood;
        //TO DO add more features to the plants. 
        ///TODO Add more stats
    };

    //we want to map the ID of the plant to our org object
    //we can access them bc the ID's are stored in our grid.
    //ID, organism
    //map<int, org> plantz;
//    map<string,map<string,string>> plantz;
    //map<int, int> plantz;

	static shared_ptr<ParameterLink<int>> modePL;
	static shared_ptr<ParameterLink<int>> numberOfOutputsPL;
	static shared_ptr<ParameterLink<int>> evaluationsPerGenerationPL;

	int mode; 
	int numberOfOutputs; 
	int evaluationsPerGeneration;

    //should have a way to figure out how to set this in the cfg file so that we can change this as needed.
    int WorldX;
    int WorldY; 
    vector<int> grid; 
    vector<org> plants; 

	WeedWorld(shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~WeedWorld() = default;

	virtual void evaluate(map<string, shared_ptr<Group> > &groups, int analyse, int visualize, int debug);

	virtual int requiredInputs() override;
	virtual int requiredOutputs() override;


    int numNeighbors(size_t vecLoc); // return number of neighbors something has in radius 1

    void feed(); // feed the plants
    void kill(); // kill the plants
    void spawn(); //spawn the plants if they're ready


    //hack hack hack (sorry) 
    bool init; //init in the eval fn only once

    //ripped out of weed world.
    // convert x,y into a grid index
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


    /*
	// takes x,y and updates them by moving one step in facing
	pair<int, int> moveOnGrid(pair<int, int> loc, int facing) {
		return {loopMod((loc.first + xm[facing]), WorldX), loopMod((loc.second + ym[facing]), WorldY)};
	}

   */

	// update value at index in grid
	void setGridValue(vector<int> &grid, int index, int value) {
		grid[index] = value;
	}

    
	// update value at x,y in grid
	void setGridValue(vector<int> &grid, pair<int, int> loc, int value) {
		setGridValue(grid, getGridIndexFromXY(loc), value);
	}
   

	vector<org> makePlants(int x, int y) {
		vector<org> grid;
		grid.resize(x * y);
        for (int i = 0; i<x*y; i++){
            org tempOrg;
            tempOrg.orgID=-1; 
            tempOrg.numFood=0; 
            tempOrg.loc=i;
            grid[i]=tempOrg; 
        }

		return grid;
	}

	// return a vector of size x*y
	vector<int> makeGrid(int x, int y) {
		vector<int> grid;
		grid.resize(x * y);
        for (int i = 0; i<x*y; i++){
            grid[i]=-1; 
        }

		return grid;
	}

    void printGrid(vector<int> &grid, ostream &os){
        cerr <<"Printing grid" <<endl; 


        //should figured out whats up with getGridValue()
        int loc =0; 
        for ( auto val : grid ){
            if (loc%WorldX==0) os<<endl;
            os << val << " "; 
            ++loc; 
        }
        os <<endl; 

        /*

        for (int i =0; i++; i < WorldX){
            for (int j=0; j++; j < WorldY){
                cout << getGridValue(grid,make_pair(i,j)) <<" "; 
            }
            cout << endl;
        }
        */

        cerr <<"done printing grid"<< endl; 
    }


	//virtual int maxOrgsAllowed() override;
	//virtual int minOrgsAllowed() override;
};



#endif /* defined(__BasicMarkovBrainTemplate__WorldWeed__) */
