//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#include "WeedWorld.h"
#include <time.h> 
#include <stdlib.h> 

shared_ptr<ParameterLink<int>> WeedWorld::modePL = Parameters::register_parameter("WORLD_TEST-mode", 0, "0 = bit outputs before adding, 1 = add outputs");
shared_ptr<ParameterLink<int>> WeedWorld::numberOfOutputsPL = Parameters::register_parameter("WORLD_TEST-numberOfOutputs", 10, "number of outputs in this world");
shared_ptr<ParameterLink<int>> WeedWorld::evaluationsPerGenerationPL = Parameters::register_parameter("WORLD_TEST-evaluationsPerGeneration", 1, "Number of times to test each Genome per generation (useful with non-deterministic brains)");

WeedWorld::WeedWorld(shared_ptr<ParametersTable> _PT) :
		AbstractWorld(_PT) {
	mode = (PT == nullptr) ? modePL->lookup() : PT->lookupInt("WORLD_TEST-mode");
	numberOfOutputs = (PT == nullptr) ? numberOfOutputsPL->lookup() : PT->lookupInt("WORLD_TEST-numberOfOutputs");
	evaluationsPerGeneration = (PT == nullptr) ? evaluationsPerGenerationPL->lookup() : PT->lookupInt("WORLD_TEST-evaluationsPerGeneration");

	// columns to be added to ave file
	aveFileColumns.clear();
	//aveFileColumns.push_back("score");
    //to do, tell archist to record things; 

    WorldX = 10;
    WorldY = 10; 
    grid = makeGrid(WorldX, WorldY); 

    init = false; //hack hack hack
}


int WeedWorld::numNeighbors(size_t vecLoc){
    //only getting things from radius 1 for now. 
    //need to expand this later.
    //copied from Emperical. Ty CAO
    
    int numNeighbors = 0; 


    //WorldX and WorldUY
    //
    for (int i = 0; i < 8; i++)
    {
        int x = (vecLoc % WorldX + i%3 - 1) % WorldX;
        int y = (vecLoc / WorldX + i/3 - 1) % WorldY;

        int pos = x + y * WorldX;

        int valAtPos = grid[pos];

        //look up to see if there is an org there.
        //-1 is the id of something that is empty
        if (valAtPos >= 0) numNeighbors +=1;


    }

    return numNeighbors;
}

// score is number of outputs set to 1 (i.e. output > 0) squared
void WeedWorld::evaluate(map<string, shared_ptr<Group>>& groups, int analyse = 0, int visualize = 0, int debug = 0) {

    if (!init){
        //orgs haven't been placed 
        std::srand(time(NULL)); 

        for (auto orgPtr : groups["default"]->population){
            int loc = rand() % (WorldX*WorldY); 
            cout <<orgPtr << " " << orgPtr->ID << " " << loc << endl; 

            
            //for now were ignoring things that collide. 
            //I'm assuming that later we'll kill them.
            //We might need to log this with somehting or whatever
            setGridValue(grid, loc, orgPtr->ID); 
            
        }


        init = true; 

        printGrid(grid,cout); 
   }
    std::cout<<"HERE"<<std::endl;


    // do these for every update
    feed(); 
    spawn(); 
    kill(); 

    cout<<groups["default"]->population.size()<< endl; 


    //get all of the orgs and put them at a spot in our grid. 
    //
    //

    
}

void WeedWorld::kill(){
    //TODO
}

void WeedWorld::spawn(){
    //TODO
}

void WeedWorld::feed(){
    //TODO
}

int WeedWorld::requiredInputs() {
	return 1;
}
int WeedWorld::requiredOutputs() {
	return numberOfOutputs;
}
