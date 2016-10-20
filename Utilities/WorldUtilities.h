//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/ahnt/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/ahnt/MABE/wiki/License

#ifndef __BasicMarkovBrainTemplate__WorldUtilities__
#define __BasicMarkovBrainTemplate__WorldUtilities__

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <set>
#include <vector>

#include "Utilities.h"

using namespace std;

class SensorArc {
public:
	class Location {
	public:
		int index;
		int locationID;
		int x;
		int y;
		int blockedIndex;
		int clearIndex;
		double distance;

		double minWorkingAngle;
		double maxWorkingAngle;

	};
	vector<int> allLocations;
	vector<Location> locationsTree;
	float workingAngle;

	inline double getworkingAngle(const double& x, const double& y) {
		double pi = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;
		return ((atan2(y, x) * (180 / pi)) + 180);
	}

	// find the angles from origin for all four corners of a 1 x 1 square centered on x,y
	// return the min and max angles (i.e. the arc this square covers)
	// if size > 1 then evaluate locations within size distance and combine arcs
	inline void getWorkingAngles(const double& x, const double& y, double& minWorkingAngle, double& maxWorkingAngle, int size = 1) {
		if (size != 1){
			cout << " in getWorkingAngles : size has not been implemented yet..." << endl;
			exit(0);
			// the idea here is to allow for blobing, for locations that are farther away, if they are blocking, they will block more area. This should reduce the size
			// of the trees and allow for farther vision (which is less accurate at distance). i.e. size = some factor of distance.
		}
		double x1 = x - .5;
		double x2 = x + .5;
		double y1 = y - .5;
		double y2 = y + .5;
		double workingAngle1 = getworkingAngle(x1, y1); // find angle to all 4 corners
		double workingAngle2 = getworkingAngle(x1, y2);
		double workingAngle3 = getworkingAngle(x2, y1);
		double workingAngle4 = getworkingAngle(x2, y2);
		//cout << x1 << " " << x2 << " " << y1 << " " << y2 << "   " << workingAngle1 << " " << workingAngle2 << " " << workingAngle3 << " " << workingAngle4 << endl;

		minWorkingAngle = min(workingAngle1, min(workingAngle2, min(workingAngle3, workingAngle4)));
		maxWorkingAngle = max(workingAngle1, max(workingAngle2, max(workingAngle3, workingAngle4)));
		if (maxWorkingAngle - minWorkingAngle > 180) { // this space crosses 360/0
			vector<double> listOnums = { workingAngle1, workingAngle2, workingAngle3, workingAngle4 };
			sort(listOnums.begin(), listOnums.end());
			minWorkingAngle = listOnums[1];
			maxWorkingAngle = listOnums[2];

		}
	}

	void drawArc(const double& distanceMax) {

		for (int y = -1 * distanceMax; y <= distanceMax; y++) {
			int Y = abs(y);
			if (Y < 10) {
				cout << "  " << Y << "  : ";
			} else if (Y < 100) {
				cout << " " << Y << "  : ";
			} else {
				cout << Y << "  : ";
			}

			for (int x = -1 * distanceMax; x <= distanceMax; x++) {
				int dist = -1;
				for (int i = 0; i < (int) allLocations.size() / 2; i++) {
					if (allLocations[i * 2] == x && allLocations[(i * 2) + 1] == y) {
						dist = i;
					}
				}
				if (dist == -1) {
					cout << " - ";
				} else if (dist < 10) {
					cout << " " << dist << " ";
				} else if (dist < 100) {
					cout << dist << " ";
				} else {
					cout << dist;
				}

			}
			cout << endl << endl;
		}

		cout << "       ";
		for (int X = -1 * distanceMax; X <= distanceMax; X++) {
			int x = abs(X);
			if (x < 10) {
				cout << " " << x << " ";
			} else if (x < 100) {
				cout << x << " ";
			} else {
				cout << x;
			}

		}
		cout << endl;
	}

	//return all of the visible arcs between angle1 and angle2 (i.e. removing arcs blocked by blocking locations.
	vector<double> makeVisableArcs(double angle1, double angle2, vector<int> blockingLocations) {

		int blockingCount = blockingLocations.size() / 2;

		vector<double> visableArcs, tempBlockedArcs, blockedArcs;

		int search_index;
		double minWorkingAngle, maxWorkingAngle;

		// get list with working angles for all blocking locations
		for (int i = 0; i < blockingCount; i++) {
			getWorkingAngles(blockingLocations[i * 2], blockingLocations[(i * 2) + 1], minWorkingAngle, maxWorkingAngle);
			if (maxWorkingAngle - minWorkingAngle < 180) {
				tempBlockedArcs.push_back(minWorkingAngle);
				tempBlockedArcs.push_back(maxWorkingAngle);
			} else {
				tempBlockedArcs.push_back(0);
				tempBlockedArcs.push_back(minWorkingAngle);
				tempBlockedArcs.push_back(maxWorkingAngle);
				tempBlockedArcs.push_back(360);
			}
		}

		// blockingCount now counts number of temp arcs!
		blockingCount = tempBlockedArcs.size() / 2;

		// sort list of blocking working angles by min value
		for (int i = 0; i < blockingCount; i++) {
			search_index = i;
			for (int j = i; j < blockingCount; j++) {
				if (tempBlockedArcs[j * 2] < tempBlockedArcs[i * 2]) {
					search_index = j;
				}
			}
			swap(tempBlockedArcs[search_index * 2], tempBlockedArcs[i * 2]);
			swap(tempBlockedArcs[(search_index * 2) + 1], tempBlockedArcs[(i * 2) + 1]);
		}

		//see if any blocking arcs can be combined or removed and define visable arcs
		if (blockingCount == 0) { // nothing blocking
			if (angle1 < angle2) {
				visableArcs = {angle1,angle2};
			} else {
				visableArcs = {0,angle2,angle1,360};
			}
		} else if (blockingCount == 1) { // there is one blocking arc
			if (angle1 < angle2) {
				if (tempBlockedArcs[0] < angle1 && tempBlockedArcs[1] > angle2) { // everything is blocked!
					visableArcs.resize(0);
				} else if (tempBlockedArcs[0] > angle1 && tempBlockedArcs[1] > angle2) { // angle1 to tempBlockedArcs[0] is visible
					visableArcs = {angle1,tempBlockedArcs[0]};
				} else if (tempBlockedArcs[0] > angle1 && tempBlockedArcs[1] < angle2) { // there are two visible arcs
					visableArcs = {angle1,tempBlockedArcs[0],tempBlockedArcs[1],angle2};
				} else if (tempBlockedArcs[0] < angle1 && tempBlockedArcs[1] < angle2) { // tempBlockedArcs[1] to angle2 is visible
					visableArcs = {tempBlockedArcs[1],angle2};
				} else { // blocking nothing
					visableArcs = {angle1,angle2};
				}
			} else { //(angle1 > angle2){
				if (tempBlockedArcs[0] > angle2 && tempBlockedArcs[1] < angle1) { // blocking nothing
					visableArcs = {0,angle2,angle1,360};
				} else if (tempBlockedArcs[0] < angle2 && tempBlockedArcs[1] < angle1) {
					visableArcs = {0,tempBlockedArcs[0],angle1,360};
				} else if (tempBlockedArcs[0] > angle2 && tempBlockedArcs[1] > angle1) {
					visableArcs = {0,angle2,tempBlockedArcs[1],360};
				} else if (tempBlockedArcs[0] > angle2 && tempBlockedArcs[1] > angle1) {
					visableArcs = {0,tempBlockedArcs[0],tempBlockedArcs[1],360};
				}
				vector<double> temp = {0,0,360,360};
				if (visableArcs == temp) { // if actually empty!
					visableArcs.resize(0);
				}
			}

		} else { // there is more then one blocking arc! build blockedArcs

			blockedArcs.push_back(tempBlockedArcs[0]);

			int j = 1;
			for (int i = 1; i < blockingCount; i = j) {
				while (j < blockingCount && tempBlockedArcs[(i*2)-1] >= tempBlockedArcs[(j*2)+1]) { // if the max of i - 1 is > the max of j, skip j
					j++;
				}
				if (j == blockingCount) { // i - 1 covers all arcs
					blockedArcs.push_back(tempBlockedArcs[(i*2)-1]);// push max of i - 1
				} else if (tempBlockedArcs[(i*2)-1] >= tempBlockedArcs[(j*2)]) { // arc j overlaps arc i - 1, new arc is min i - 1 to max j
					//do nothing, we need to see if j max is the end of an arc, or if it is in j+1
				} else { // no overlap, push max of i - 1 (to end last arc, and min of j to start new arc
					blockedArcs.push_back(tempBlockedArcs[(i*2)-1]);// push max of i - 1
					blockedArcs.push_back(tempBlockedArcs[(j*2)]);// push min of j
				}
				j++;

				if (j == blockingCount) { // if we happen to be on the last one, push the last value
					blockedArcs.push_back(tempBlockedArcs[(j*2)-1]);// push max of j
				}
			}

			// blockingCount now counts number elements (not arcs)
			blockingCount = blockedArcs.size();

			int tempVisableArcsCount;
			vector<double> tempVisableArcs;
			if (angle1 < angle2) {
				tempVisableArcs = {angle1,angle2};
				tempVisableArcsCount = 2;
			} else {
				tempVisableArcs = {0,angle2,angle1,360};
				tempVisableArcsCount = 4;
			}

			// Now, start counters v and b at 0, march though visableArcs and blockedArcs
			int v = 0;
			int b = 0;

			bool inVisArc = false;

			double v1 = tempVisableArcs[v++];
			double v2 = tempVisableArcs[v];

//			while (b < blockingCount && blockedArcs[b] < v1) {
//				b++;
//			}

			if (b == blockingCount || blockedArcs[0] > tempVisableArcs[tempVisableArcsCount-1]) {
				visableArcs = tempVisableArcs;
			} else {

				while (v < tempVisableArcsCount && b < blockingCount) {

					if (!inVisArc) { // if we are between visible arcs
						if (blockedArcs[b] < v1) { // if this b is < the start of the next visible arc, skip it
							b++;
						} else if (blockedArcs[b] == v1 && b % 2 == 0) {
							b++;
							if (blockedArcs[b] > v1 && blockedArcs[b] < v2) {
								visableArcs.push_back(blockedArcs[b]);
								inVisArc = true;
								b++;
							}
						} else if (b%2 == 0) { // if b is in arc, and is a start of a blocking arc
							visableArcs.push_back(v1);// add v1 and set inVisArc true (we are now in an arc
							inVisArc = true;
						} else if (blockedArcs[b] < v2) { // if b in arc and it is an end, set it (the start of the visible arc is hidden
							visableArcs.push_back(blockedArcs[b]);
							b++;
							inVisArc = true;
						} else if (b%2 == 0) { // if the next blocking arc is past the end of this visible arc, add v1 and v2, the whole arc, and we are still !inVisArc
							visableArcs.push_back(v1);
							visableArcs.push_back(v2);
							v++;
							if (v < tempVisableArcsCount) { // if there are more vis arcs, advance
								v1 = tempVisableArcs[v++];
								v2 = tempVisableArcs[v];
							}
						} else { // this ends the current block ... this whole visual arc is not seen
							v++;
							if (v < tempVisableArcsCount) { // if there are more vis arcs, advance
								v1 = tempVisableArcs[v++];
								v2 = tempVisableArcs[v];
							}
						}
					} else { // inVisArc
						if (blockedArcs[b] <= v1) { // if b is not in the next vis arc, skip it
							b++;
						} else if (b%2 == 0 && blockedArcs[b] < v2) { // if b is the start of a blocked arc and in this vis arc, add it
							visableArcs.push_back(blockedArcs[b]);
							b++;
						} else if (b%2 == 1 && blockedArcs[b] < v2) { // if b is the end of a blocked arc and in this vis arc, add it
							visableArcs.push_back(blockedArcs[b]);
							b++;
						} else if (b%2 == 0 && blockedArcs[b] > v2) { // if b is the start of a blocked arc and past the end of this vis arc...
							visableArcs.push_back(v2);// we are in the clear, just add v2...
							inVisArc = false;// we are now between vis arcs
							v++;
							if (v < tempVisableArcsCount) { // if there are more vis arcs, advance
								v1 = tempVisableArcs[v++];
								v2 = tempVisableArcs[v];
							}
						} else if (b%2 == 1 && blockedArcs[b] >= v2) { // if b is the end of a blocked arc and past the end of this vis arc...
							inVisArc = false;// then the last b closed this arc, skip v2. also, we are now between vis arcs
							v++;
							if (v < tempVisableArcsCount) { // if there are more vis arcs, advance
								v1 = tempVisableArcs[v++];
								v2 = tempVisableArcs[v];
							}
						}
					}
				}
			}

			// now check to see if there are any viable arcs left and add if needed
			if (b == blockingCount) {
				if (inVisArc) { // if we are in an arc currently, we need to close it out and advance
					visableArcs.push_back(v2);
					v++;
					if (v < tempVisableArcsCount) { // if there are still arcs, advance and add
						v1 = tempVisableArcs[v++];
						v2 = tempVisableArcs[v];
					}
				}
				while (v < tempVisableArcsCount) { // while there are still arcs, advance and add
					visableArcs.push_back(v1);
					visableArcs.push_back(v2);
					v++;
					if (v < tempVisableArcsCount) {
						v1 = tempVisableArcs[v++];
						v2 = tempVisableArcs[v];
					}

				}
			}

//			int visableArcsCount = 1;

		}
		return (visableArcs);
	}

	// given the x and y of a location (square), is that square visible (in visibleArcs)
	bool isLocationVisible(const vector<double>& visableArcs, const int& x, const int& y) {
		double minWorkingAngle, maxWorkingAngle, arc1, arc2;
		getWorkingAngles(x, y, minWorkingAngle, maxWorkingAngle);

		bool visible = false;
		for (int i = 0; i < ((int) visableArcs.size()) / 2; i++) {
			arc1 = visableArcs[i * 2];
			arc2 = visableArcs[(i * 2) + 1];

			if ((minWorkingAngle > arc1 && minWorkingAngle < arc2) || (maxWorkingAngle > arc1 && maxWorkingAngle < arc2)) { // if either of the min or max are in this visible arc...
				visible = true;
			} else {
				if (maxWorkingAngle - minWorkingAngle <= 180) { // else if the min and max do not cross 0/360 and contain a visible arc...
					if (minWorkingAngle < arc1 && maxWorkingAngle > arc2) {
						visible = true;
					}
				} else { // if min and max do cross 360 and...
					if (minWorkingAngle + 360 > arc2 && maxWorkingAngle < arc1) { // if the max to min + 360 (super arc?) contains a visible arc...
						visible = true;
					} else if (minWorkingAngle > arc2 && maxWorkingAngle - 360 < arc1) { // if the max - 360 to min (super arc in the negative?) contains a visible arc...
						visible = true;
					}
				}
			}
		}
		return (visible);
	}


	// create a SensorArc Binary Decision Tree
	// the arc will be from angle1 to angle2 (order matters) and will extend distance from the origin, locations closer then distanceMin are not seen
	// if noBlocking, then
	SensorArc(double angle1, double angle2, double distanceMax, double distanceMin, bool noBlocking = false) {
		//vector<int> allLocations;
		int locationsCount;

		double distanceMaxSquared = pow(distanceMax, 2);
		double distanceMinSquared = pow(distanceMin, 2);

		double minWorkingAngle, maxWorkingAngle;

		bool check;

		if (angle1 == -180 && angle2 == 180) {
			angle1 = 0;
			angle2 = 360;
		} else {
			// conversion to get values between 0 and 360 with 0 degrees up
			angle1 += 90;
			angle2 += 90;
			angle1 = loopMod(angle1, 360);
			angle2 = loopMod(angle2, 360);
		}

		// first get a list of locations

		for (int y = -1 * distanceMax; y <= distanceMax; y++) {
			for (int x = -1 * distanceMax; x <= distanceMax; x++) {
				check = false;

				getWorkingAngles(x, y, minWorkingAngle, maxWorkingAngle);

				// if any corner is in between angles, set check to true
				if (angle1 <= angle2) {
					//cout << minWorkingAngle << "  " << maxWorkingAngle << "    " << angle1 << "  " << angle2 << endl;
					if (minWorkingAngle > angle1 && minWorkingAngle < angle2 && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location is in the arc and distance
					//cout << "min" << endl;
						check = true;
					} else if (maxWorkingAngle > angle1 && maxWorkingAngle < angle2 && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location is in the arc and distance
					//cout << "max" << endl;
						check = true;
					} else if (maxWorkingAngle - minWorkingAngle <= 180) {
						if (minWorkingAngle <= angle1 && maxWorkingAngle >= angle2 && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) {
							//cout << "mid" << endl;
							check = true;
						}
					} else { // maxWorkingAngle - minWorkingAngle > 180
						if ((minWorkingAngle > angle1 || maxWorkingAngle < angle2) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) {
							//cout << "outside" << endl;
							check = true;
						}
					}
				} else { //angle1 > angle2
					if ((minWorkingAngle < angle2 || minWorkingAngle > angle1) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location is in the arc and distance
						check = true;
					} else if ((maxWorkingAngle < angle2 || maxWorkingAngle > angle1) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) { // if this location is in the arc and distance
						check = true;
					} else if (maxWorkingAngle - minWorkingAngle <= 180) {
						if ((minWorkingAngle <= angle2 && maxWorkingAngle >= angle1) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) {
							check = true;
						}
					} else { // maxWorkingAngle - minWorkingAngle > 180
						if ((minWorkingAngle > angle2 || maxWorkingAngle < angle1) && (pow(x, 2) + pow(y, 2)) <= distanceMaxSquared && (pow(x, 2) + pow(y, 2)) >= distanceMinSquared) {
							check = true;
						}
					}
				}

				if (check) { // if any of the corners are between the angles, add that point to allLocations
					cout << x << " " << y << endl;
					allLocations.push_back(x);
					allLocations.push_back(y);
				}

			}
		}

		locationsCount = allLocations.size() / 2;

		// now we have a list of locations, sort the list
		int search_index;

		for (int i = 0; i < locationsCount; i++) { // for every location
			search_index = i;
			for (int j = i + 1; j < locationsCount; j++) { // for every location after i
				if ((pow(allLocations[j * 2], 2) + pow(allLocations[j * 2 + 1], 2)) < (pow(allLocations[search_index * 2], 2) + pow(allLocations[search_index * 2 + 1], 2))) {
					search_index = j; // if the location of j is closer, j is now closest
				}
			}
			// swap closest pair to current
			swap(allLocations[i * 2], allLocations[search_index * 2]);
			swap(allLocations[i * 2 + 1], allLocations[search_index * 2 + 1]);
		}

		// now build the locationsTree!

		int allLocations_index = 0;

		vector<int> blockingLocations;

		locationsTree.clear();
		blockingLocations.clear();

		int allLocationsSize = allLocations.size() / 2;
		makeLocationsTree(allLocationsSize, blockingLocations, allLocations_index, noBlocking);

//		for (auto l : locationsTree) {
//			cout << l.index << " -> " << l.locationID << "  <>  " << l.x << "," << l.y << "   " << l.clearIndex << " : " << l.blockedIndex << "    " << l.maxWorkingAngle << "," << l.minWorkingAngle << endl;
//		}
//
//		drawArc(distanceMax, allLocations);
	}

	// make binary decision tree
	void makeLocationsTree(const int& allLocationsSize, vector<int> blockingLocations, int allLocations_index, bool noBlocking) {
		if (allLocations_index < allLocationsSize) { // if we have not passed the last element in allLocations...
			// if not blocked, add current location to locationsTree
			Location newLocation;
			int currentLocationTree_index = ((int) locationsTree.size()); // capture this index, we will need it later
			newLocation.index = currentLocationTree_index;
			newLocation.locationID = allLocations_index;
			newLocation.x = allLocations[allLocations_index * 2];
			newLocation.y = allLocations[(allLocations_index * 2) + 1];
			newLocation.distance = sqrt((pow(newLocation.x, 2) + pow(newLocation.y, 2)));

			locationsTree.push_back(newLocation);
			allLocations_index++;

			// now check locations from allLocations (starting at allLocations_index+1) till a visible location is found, if non is found, mark newLocation.clearIndex = -1
			// else newLocation.clearIndex = currentLocationTree_index+1; // this is where to go if this location is clear
			// then call makeLocationsTree(allLocations, blockedLocations, index of non-blocked location)

			vector<double> visableArcs = makeVisableArcs(0, 360, blockingLocations);

			int tempAllLocations_index = allLocations_index;
			while (allLocations_index < allLocationsSize && !isLocationVisible(visableArcs, allLocations[allLocations_index * 2], allLocations[(allLocations_index * 2) + 1])) {
				// while there are locations on allLocations and they are not visible...
				allLocations_index++;
			}

			// we should now either be done, or have a visible location

			if (allLocations_index == allLocationsSize) {
				locationsTree[currentLocationTree_index].clearIndex = -1; // with this location clear, there are no other clear locations, we are at a leaf node
			} else { // else we have a visible location, add it to the tree
				locationsTree[currentLocationTree_index].clearIndex = currentLocationTree_index + 1;
				makeLocationsTree(allLocationsSize, blockingLocations, allLocations_index, noBlocking);
			}

			// once we come back ...

			if (noBlocking) {
				if (allLocations_index < allLocationsSize){
					locationsTree[currentLocationTree_index].blockedIndex = currentLocationTree_index + 1;
				} else {
					locationsTree[currentLocationTree_index].blockedIndex = -1;
				}
			} else {// continue here if we need to precalculate blocking
				// add this location to blockingLocations
				blockingLocations.push_back(locationsTree[currentLocationTree_index].x);
				blockingLocations.push_back(locationsTree[currentLocationTree_index].y);
				visableArcs = makeVisableArcs(0, 360, blockingLocations);

				// (yes, this is a repeat)
				// now check locations from allLocations (starting at allLocations_index+1) till a visible location is found, if non is found, mark newLocation.blockIndex = -1
				// else newLocation.blockIndex = locationsTree.size(); // this is where to go if this location is blocked, i.e. to the end of the locationTree
				// then call makeLocationsTree(allLocations, blockedLocations, index of non-blocked location, locationsTree_index)

				allLocations_index = tempAllLocations_index;
				while (allLocations_index < allLocationsSize && !isLocationVisible(visableArcs, allLocations[allLocations_index * 2], allLocations[(allLocations_index * 2) + 1])) {
					// while there are locations on allLocations and they are not visible...
					allLocations_index++;
				}

				// we should now either be done, or have a visible location

				if (allLocations_index == allLocationsSize) {
					locationsTree[currentLocationTree_index].blockedIndex = -1; // with this location clear, there are no other clear locations, we are at a leaf node
				} else { // else we have a visible location, add it to the tree
					locationsTree[currentLocationTree_index].blockedIndex = (int) locationsTree.size();
					makeLocationsTree(allLocationsSize, blockingLocations, allLocations_index, noBlocking);
				}
			}
		}
	}
};

class Sensor {
public:
	int resolution; // how many angles to precalculate

	map<int,shared_ptr<SensorArc>> angles;

	Sensor(double angle1, double angle2, double distanceMax, double distanceMin, int _resolution, bool noBlocking = false){
		resolution = _resolution;
		double resolutionOffset = 360/resolution;
		for (int i = 0; i < resolution; i++){
			angles[i] = make_shared<SensorArc>((i*resolutionOffset)+angle1,(i*resolutionOffset)+angle2,distanceMax,distanceMin,noBlocking);
		}
	}

};

#endif // __BasicMarkovBrainTemplate__WorldUtilities__