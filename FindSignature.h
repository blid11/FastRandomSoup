#pragma once

#ifndef FindSignature_h
#define FindSignature_h
#endif /* FindSignature_h */

#include <map>
#include <iostream>
#include <algorithm>
#include "LookupNeighbours.h"
#include "LrgLookup.h"
#include "DetectFalsePositive.h"

class FindSignature {

public:
	//ATTRIBUTES
	short gridLength;
	short cellThtHitBorder;
	vector<short> liveCellsLastGen;
	vector<vector<short>> nonFPIndices;

	LookupNeighbours* lookup2;
	LrgLookup* lrgLookUP;

	int numOfLiveCells;

	vector<short> neighbourCellVector;
	vector<short> cellsToRunProcedureOn;

	map<short, short> liveNeighbourCountAllCells;
	map<short, short> aliveOrDeadMap;

	vector<short> numLiveCellINbour;
	vector<short> numDeadCellINbour;
	vector<short> signature;

	vector<short> liveCellsOfObject;

	short whatHitBorder;
	bool isFalsePositive;

	//FUNCTIONS
	FindSignature(vector<short> liveCellsCurrent, short cellThatHitBorder, short gridDimension, LookupNeighbours* lookUpTable, LrgLookup* lrgLookUP);

	void requestNeighbourStrings(short addressOfCell);
	void liveNeighbourCellCount();
	void checkIfLiveOrDead();
	void fillLiveCellCountNDeadAlive();
	void fillInSignature();
	void compareSignature();
	void detectFalsePos();

	void printSignature();
	int getSignatureNumber();
	bool getFalsePos();

};

FindSignature::FindSignature(vector<short> liveCellsCurrent, short cellThatHitBorder, short gridDimension, LookupNeighbours* lookUpTable, LrgLookup* lrgLook) {

	gridLength = gridDimension;
	lookup2 = lookUpTable;
	lrgLookUP = lrgLook;

	numOfLiveCells = 0;

	liveCellsLastGen = liveCellsCurrent;
	cellThtHitBorder = cellThatHitBorder;

	whatHitBorder = 0;
	isFalsePositive = true;

	numLiveCellINbour = vector<short>(26, 0);
	numDeadCellINbour = vector<short>(26, 0);;

	//function 1: for first border cell
	requestNeighbourStrings(cellThatHitBorder);

	//function 2
	liveNeighbourCellCount();

	//function 3: make vector cellsToRunProcedureOn
	checkIfLiveOrDead();

	neighbourCellVector.clear();

	//now the following function will do the same 3 steps for every address in the vector cellsToRunProcedureOn
	//now do the same 3 functions in a while loop to fill both liveNeighbourCountAllCells and aliveOrDeadMap
	fillLiveCellCountNDeadAlive();

	//once the maps that contain the cell count and live/dead status of each cell have been filled,
	//can fill the vector that denotes the signature of what hit the border
	fillInSignature();

	compareSignature();

	//printSignature();

	//an int, denoting whether the signature was the same as or different from a known glider can then be requested by an external class
	if (whatHitBorder == 2) {
	detectFalsePos();
	short intHolder = 0;
	vector<short>::iterator printNumber = signature.begin();

	cout << endl << "(";
	while (printNumber != signature.end()) {
		intHolder = *printNumber;
		cout << intHolder << ", ";
		printNumber++;
	}
	cout << ")";
	cout << endl;
	}
	else {

	}


}

void FindSignature::fillLiveCellCountNDeadAlive() {

	short indexOfCell = 0;

	for (size_t iter = 0; iter < cellsToRunProcedureOn.size(); iter++) {

		//take the first values address as a string and store it in decodeThis
		indexOfCell = cellsToRunProcedureOn[iter];

		requestNeighbourStrings(indexOfCell);

		liveNeighbourCellCount();

		checkIfLiveOrDead();

		neighbourCellVector.clear();

	}

}

void FindSignature::requestNeighbourStrings(short addressOfCell) {

	short* pointsToNbours = lookup2->getNeighbours(addressOfCell);
	short nbourIndex = 0;

	for (int nbour = 0; nbour < 26; nbour++) {
		nbourIndex = pointsToNbours[nbour];
		neighbourCellVector.push_back(nbourIndex);
	}

	//the issue here is that you're trying to ask for a cell address that doesn't exist?

	//since this procedure is only ever run for the live cells within the object's vicinity
	//place the address in the vector below so it can be used to test for false positive

}

void FindSignature::liveNeighbourCellCount() {

	//this function is only called to process the neighbours of live cells
	//the result is a map that lists the no. of live neighbours for all cells, live or dead in the vicinity of the glider

	short nbourIndex = 0;

	//iterator to iterate over the vector filled with the neighbour cells of the cell being censused
	vector<short>::iterator iteratorForCounting = neighbourCellVector.begin();

	map<short, short>::iterator updateInt;

	while (iteratorForCounting != neighbourCellVector.end()) {
		nbourIndex = *iteratorForCounting;
		updateInt = liveNeighbourCountAllCells.find(nbourIndex);
		if (updateInt != liveNeighbourCountAllCells.end()) {
			updateInt->second = updateInt->second + 1;
		}
		else {
			liveNeighbourCountAllCells.insert(pair<short, short>(nbourIndex, 1));
		}
		iteratorForCounting++;
	}

}

void FindSignature::checkIfLiveOrDead() {

	//need to check if the neighbours of the cells are alive or dead before we add them to the vector cellsToRunProcedureOn
	//concurrently, add the cells that aren't present to a new map with an int value 0
		//this will allow you to quickly determine if a cell is dead (if it found in this map) when deciding which element of the signature to increment

	short nbourIndex = 0;

	vector<short>::iterator iteratorForAliveDead = neighbourCellVector.begin();

	while (iteratorForAliveDead != neighbourCellVector.end()) {
		nbourIndex = *iteratorForAliveDead;

		if (aliveOrDeadMap.count(nbourIndex)) {
			iteratorForAliveDead++;
		}

		else {
			if (find(liveCellsLastGen.begin(), liveCellsLastGen.end(), nbourIndex) != liveCellsLastGen.end()) {
				aliveOrDeadMap.insert(pair<short, short>(nbourIndex, 1));
				cellsToRunProcedureOn.push_back(nbourIndex);
			}
			else {
				aliveOrDeadMap.insert(pair<short, short>(nbourIndex, 0));
				//cout<<searchThisAddress<<endl;
				//the problem is that all the cells are showing up as dead
				//this may be because the last gen map is not being accessed?
				//or is the cell that hit border from a generation prior to lastGen
			}
			iteratorForAliveDead++;
		}

	}

}



void FindSignature::fillInSignature() {

	//first need two vectors: no of live cells w/ i live neighbours, no of dead cells w/ i live neighbours (i cannot be 0 for this one)
	//before the count of cells w/i live neighbours is incremented, need to check which vector in which to increment the value: search its address

	short nbourIndex = 0;

	short countHolder = 0;

	short stateHolder = 0;

	map<short, short>::iterator iterateOverNeighbourCount;

	map<short, short>::iterator iterateOverAliveDead;

	for (int checkForThisNum = 1; checkForThisNum < 27; checkForThisNum++) {

		for (iterateOverNeighbourCount = liveNeighbourCountAllCells.begin(); iterateOverNeighbourCount != liveNeighbourCountAllCells.end(); iterateOverNeighbourCount++) {

			countHolder = iterateOverNeighbourCount->second;

			if (checkForThisNum == countHolder) {
				//want to increment the no. of cells either in the live or dead cell vector

				nbourIndex = iterateOverNeighbourCount->first;
				iterateOverAliveDead = aliveOrDeadMap.find(nbourIndex);
				stateHolder = iterateOverAliveDead->second;

				if (stateHolder) {
					//if the cell is alive
					//increment the count for
					numLiveCellINbour[checkForThisNum] += 1;
					//FOR DETECT FALSE POSITIVE 
					liveCellsOfObject.push_back(nbourIndex);
					numOfLiveCells++;

				}
				else {
					numDeadCellINbour[checkForThisNum] += 1;
				}
			}

			else {
				continue;
			}


		}

	}

	signature = vector<short>(numLiveCellINbour);
	signature.insert(signature.end(), numDeadCellINbour.begin(), numDeadCellINbour.end());

}


void FindSignature::detectFalsePos() {

	//will create a DetectFalsePositive object. This is like RunXSimulations
	//will creat a Simulation and will contain functions that will make the vector<int*> as input (the live cells that hit the border)
	//this class also performs operations prior to running the Simulation however: it centres whatever live cells were on the border


	DetectFalsePositive detectFP = DetectFalsePositive(lookup2, lrgLookUP, liveCellsOfObject, numOfLiveCells, cellThtHitBorder, gridLength);
	//creates and runs a Simulation until it is finished, will change the value isFalsePositive
	//STILL NEED to store grid stuff in this class to pass to this function

	isFalsePositive = detectFP.getFalsePosInfo();

	/*
	if (isFalsePositive) {

	}
	else {
		nonFPIndices = detectFP->getIndices();
		printSignature();
	}
	*/

	//if(isFalsePositive != true){
		//detectFP->printShiftedAddresses();
	//}
	//else{

	//}

}


void FindSignature::compareSignature() {

	//a glider only moves when it is at phase 4 or 8 and so we see the same signatures for all 4 phases

	//the signature for glider v is:

	vector<short> gliderVSig1 = { 0, 0, 0, 2, 0, 3, 1, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 18, 12, 8, 4, 1, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	vector<short> gliderVSig2 = { 0, 0, 0, 2, 0, 3, 1, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 26, 12, 12, 4, 1, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	//the signature for glider e is:
	vector<short> gliderESig = { 0, 0, 0, 2, 8, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 37, 53, 24, 24, 8, 6, 0, 6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	//now just need to compare the vector of the signature for a given simulation and these vectors
	//depending on the value will return a different int for whatHitBorder

	if (gliderVSig1 == signature || gliderVSig2 == signature) {
		whatHitBorder = 0;
	}
	else if (gliderESig == signature) {
		whatHitBorder = 1;
	}
	else {
		whatHitBorder = 2;
	}

}

int FindSignature::getSignatureNumber() {
	return whatHitBorder;
}

bool FindSignature::getFalsePos() {
	return isFalsePositive;
}

void FindSignature::printSignature() {

	short intHolder = 0;
	vector<short>::iterator printNumber = signature.begin();

	cout << endl << "(";
	while (printNumber != signature.end()) {
		intHolder = *printNumber;
		cout << intHolder << ", ";
		printNumber++;
	}
	cout << ")";
	cout << endl;

	vector<vector<short>>::iterator printIndices = nonFPIndices.begin();

	vector<short> indexHolder(3);


	while (printIndices != nonFPIndices.end()) {
		indexHolder = *printIndices;
		cout << "(";
		cout << indexHolder[0] << ", ";
		cout << indexHolder[1] << ", ";
		cout << indexHolder[2];
		cout << ")";
		printIndices++;
	}
	cout << endl;


}

