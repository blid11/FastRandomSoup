

#pragma once

#ifndef DetectFalsePositive_h
#define DetectFalsePositive_h


#endif /* DetectFalsePositive_h */

#include "LrgLookup.h"

class DetectFalsePositive {

public:

	//Attributes
	LrgLookup* lrgLook;
	LookupNeighbours* lookup3;
	short firstBorder;
	int noOfLiveCells;
	int gridDim;
	vector<short> objectCellAddresses;
	vector<vector<short>> shiftedAddresses;
	bool stillLife;
	bool hitBorder;
	vector<long> liveCellCurrent;
	vector<long> liveCellPrev;
	unordered_map<long, short> relevantCells;
	unordered_map<long, short> incDecCells;
	int numLiveCells;
	bool falsePositive;

	//Functions

	DetectFalsePositive(LookupNeighbours* lookup2, LrgLookup* lrgLookup, vector<short> liveCellsOfObject, int numOfLiveCells, short cellThtHitBorder, short gridLength);

	vector<short> getIndexVector(short convertThis);
	void shiftCellAddresses();
	void runExperiment();
	void makeNewSim();
	void performUpdate();
	bool getFalsePosInfo();
	void printNotFP();
	vector<vector<shorgt>> getIndices();

};

DetectFalsePositive::DetectFalsePositive(LookupNeighbours* lookup2, LrgLookup* lrgLookup, vector<short> liveCellsOfObject, int numOfLiveCells, short cellThtHitBorder, short gridLength) {

	//how to find the centreMost cell of whatever configuration hit the border
	//don't necessarily need to do that: for now just use whatever cell hit the border

	//find the difference between the cell that hit the border and the centre cell in terms of i, j, k
	//shift all the cells in liveCellsNearBorder by the difference vector for i, j, k

	lookup3 = lookup2;

	firstBorder = cellThtHitBorder;

	objectCellAddresses = liveCellsOfObject;

	noOfLiveCells = numOfLiveCells;

	gridDim = 75;

	falsePositive = true;

	lrgLook = lrgLookup;

	shiftCellAddresses();

	runExperiment();

	if (falsePositive == false) {
		printNotFP();
	}
	else {
	}

}

void DetectFalsePositive::shiftCellAddresses() {

	short centredAddress[3] = { 0, 0, 0 };
	short differenceVector[3] = { 0, 0, 0 };
	vector<short> theBorderCell(3);

	theBorderCell = getIndexVector(firstBorder);

	centredAddress[0] = (gridDim / 2 + 1);
	centredAddress[1] = (gridDim / 2 + 1);
	centredAddress[2] = (gridDim / 2 + 1);

	differenceVector[0] = centredAddress[0] - theBorderCell[0];
	differenceVector[1] = centredAddress[1] - theBorderCell[1];
	differenceVector[2] = centredAddress[2] - theBorderCell[2];

	short index = 0;

	vector<short> indexVector(3);

	//for(size_t iterOverLiveCells = 0; iterOverLiveCells < objectCellAddresses.size(); iterOverLiveCells++){
	for (vector<short>::iterator iter = objectCellAddresses.begin(); iter != objectCellAddresses.end(); iter++) {

		//index = objectCellAddresses[iterOverLiveCells];
		index = *iter;

		indexVector = getIndexVector(index);

		//cout<<"indices pre shift: ("<< indicesHolder[0] <<", "<<indicesHolder[1] <<", "<< indicesHolder[2] << ")";

		vector<short>addAddressToVector(3);

		for (int indexNumber = 0; indexNumber < 3; indexNumber++) {

			if (indexNumber == 0) {
				addAddressToVector[0] = differenceVector[0] + indexVector[0];
			}
			else if (indexNumber == 1) {
				addAddressToVector[1] = differenceVector[1] + indexVector[1];
			}
			else {
				addAddressToVector[2] = differenceVector[2] + indexVector[2];
			}

		}

		//cout<< "shifted cell index: ("<< addAddressToVector[0] <<", "<<addAddressToVector[1] <<", "<< addAddressToVector[2] << ")";

		shiftedAddresses.push_back(addAddressToVector);

	}

	//cout<<endl;

}

vector<short> DetectFalsePositive::getIndexVector(short convertThis) {

	//this function will take an int, and put its binary representation in a cstring
	//the char array is set to {0,0,0,0,0} to start
	//the first value is placed in the last spot of the array

	short convert = convertThis;

	if (convert < 0) {
		convert = -convert;
	}
	else {
	}

	short copyToArray[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	unsigned short remainder = 0;
	int quotient = 0;

	int counter = 15;

	//by filling all 5 positions no matter what, starting with the rightmost
	//will fill the 5th position with a 0 even if the quotient and remainder have already been 0
	//this will be allow for easy conversion back to decimal later.
	while (counter != -1) {

		quotient = convert / 2;
		remainder = convert % 2;
		convert = quotient;

		//the last position has the last part of the binary rep placed in it

		copyToArray[counter] = remainder;

		counter--;

	}

	//now that the array has been filled, iterate over it
	//for the first 5 values after 0th position, take the no in binary and make it a short
	//place the result in the first position of the vector(3)

	short x = 0;
	short y = 0;
	short z = 0;
	x = +(copyToArray[5]) * 1 + (copyToArray[4]) * 2 + (copyToArray[3]) * 4 + (copyToArray[2]) * 8 + (copyToArray[1]) * 16;
	y = +(copyToArray[10]) * 1 + (copyToArray[9]) * 2 + (copyToArray[8]) * 4 + (copyToArray[7]) * 8 + (copyToArray[6]) * 16;
	z = +(copyToArray[15]) * 1 + (copyToArray[14]) * 2 + (copyToArray[13]) * 4 + (copyToArray[12]) * 8 + (copyToArray[11]) * 16;

	vector<short> indices = { x,y,z };

	return indices;

}

void DetectFalsePositive::runExperiment() {

	stillLife = false;
	hitBorder = false;

	bool oscillator = false;
	int genNumber = 0;

	numLiveCells = noOfLiveCells;

	makeNewSim();

	/*
	long printThis = 0;
	cout << "Live cells 1st gen are:" << endl;
	for (vector<long>::iterator iterLive = liveCellPrev.begin(); iterLive != liveCellPrev.end(); iterLive++) {
		printThis = *iterLive;
		cout << printThis << endl;
	}
	*/

	while ((!hitBorder) && numLiveCells && (!oscillator) && (!stillLife)) {

		//cout<<"updating generation: "<<genNumber<<endl;

		performUpdate();
		genNumber++;

		if (hitBorder) {
			falsePositive = false;
			//cout << "hit border is end condition" << endl; 
			break;
		}
		else if (genNumber > 200) {
			falsePositive = true;
			break;
		}
		else if (liveCellCurrent == liveCellPrev) {
			//stillLife = true;
			falsePositive = true;
			break;
		}
		else if (numLiveCells == 0) {
			falsePositive = true;
			break;
		}
		else {
		}

		liveCellPrev = liveCellCurrent;

		/*
		long printThis = 0;
		cout << "Live cells CURRENT are:" << endl;
		for (vector<long>::iterator iterLive = liveCellCurrent.begin(); iterLive != liveCellCurrent.end(); iterLive++) {
			printThis = *iterLive;
			cout << printThis << endl;
		}
		cout << endl;
		*/

		liveCellCurrent.clear();

	}

}

void DetectFalsePositive::makeNewSim() {

	vector<vector<short>>::iterator iter;
	vector<short> encodeThis(3);
	long indexToAdd = 0;

	int x, y, z = 0;

	for (iter = shiftedAddresses.begin(); iter != shiftedAddresses.end(); iter++) {
		encodeThis = *iter;

		x = encodeThis[0];
		y = encodeThis[1];
		z = encodeThis[2];

		indexToAdd = lrgLook->getAddressEncoded(x, y, z);

		liveCellPrev.push_back(indexToAdd);

		//step 1: check if this value is present in the map as a neighbour

		auto iter2 = relevantCells.find(indexToAdd);

		if (iter2 != relevantCells.end()) {

			//get the neighbour and state value for this cell
			//the state should now be set to on, its neighbour count does not increase
			//as whatever live cells have added it as a neighbour have incremented its value
			short editNStat = iter2->second;
			//this ensures you don't set it to 1 more than once aka = 2
			editNStat = (editNStat / 10) * 10 + 1;
			iter2->second = editNStat;

		}
		else {
			//the 1 means state 1
			short addNStat = 1;
			relevantCells.insert(pair<long, short>(indexToAdd, addNStat));

		}

		long* pointsToNeighbours = lrgLook->getNeighbours(indexToAdd);

		short editNCount = 0;
		short editState = 0;

		for (int neighbo = 0; neighbo < 26; neighbo++) {

			long nToAdd = pointsToNeighbours[neighbo];

			iter2 = relevantCells.find(nToAdd);

			if (iter2 != relevantCells.end()) {
				//it is either present as a neighbour or a live cell
				editNCount = (iter2->second) / 10;

				//

				editState = (iter2->second) % 10;
				//preserve info about state and neighbour count but increment n count
				editNCount = (editNCount + 1) * 10 + editState;

				iter2->second = editNCount;

			}
			else {
				//give it a neighbour count of 1 and state 0 represented by 10
				short addNStat = 10;
				relevantCells.insert(pair<long, short>(nToAdd, addNStat));
				//cout<<nToAdd<<": new :" << neighbo <<endl;

			}

		}

	}

}


void DetectFalsePositive::performUpdate() {

	//now the map has been filled for the first generation
	//print out all the values
	unordered_map<long, short>::iterator updateStatCount = relevantCells.begin();

	long index = 0;
	short statCount = 0;
	short state = 0;
	short count = 0;

	long nbourIndex = 0;

	unordered_map<long, short>::iterator findNeighbour = incDecCells.begin();

	short incDecCount = 0;

	int cellCount = 0;

	while (updateStatCount != relevantCells.end()) {

		index = updateStatCount->first;
		statCount = updateStatCount->second;
		count = statCount / 10;
		state = statCount % 10;

		if (state && ((count == 5) || (count == 6) || (count == 7))) {
			liveCellCurrent.push_back(index);
			cellCount++;
			//cout<<index<<"lived, state: "<< state << ", nCount: "<<count<<endl;

		}
		else if (state) {
			//first change the state of the cell itself to 0
			statCount = count * 10;
			updateStatCount->second = statCount;
			//cout<<index<<"died, state: "<< state << ", nCount: "<<count<< ", statCount"<<updateStatCount->second<< endl;

			//the cell died and the nCount for its neighbours must be decremented to 0
			long* pointsToNbours = lrgLook->getNeighbours(index);
			//AS THE CELL WAS LIVE, ALL NEIGHBOURS SHOULD BE IN MAP
			for (int nbour = 0; nbour < 26; nbour++) {
				nbourIndex = pointsToNbours[nbour];
				//neighbour index iterator points to the map of cells to increment and decrement
				findNeighbour = incDecCells.find(nbourIndex);
				if (findNeighbour != incDecCells.end()) {
					//if this cell is already on the list of cells to inc/dec
					incDecCount = (findNeighbour->second) - 1;
					findNeighbour->second = incDecCount;
				}
				else {
					incDecCells.insert(pair<long, short>(nbourIndex, -1));
				}

			}

		}
		else if (count == 6) {
			liveCellCurrent.push_back(index);
			cellCount++;

			if (index <= 0) {
				hitBorder = true;
			}
			else {

			}

			//change the state to 1
			statCount = count * 10 + 1;
			updateStatCount->second = statCount;
			//cout<<index<<"born, state: "<< state << ", nCount: "<<count<< ", statcount" <<statCount<<endl;
			//this works fine

			long* pointsToNbours = lrgLook->getNeighbours(index);
			//AS THE CELL WAS LIVE, ALL NEIGHBOURS SHOULD BE IN MAP
			for (int nbour = 0; nbour < 26; nbour++) {
				nbourIndex = pointsToNbours[nbour];
				findNeighbour = incDecCells.find(nbourIndex);
				if (findNeighbour != incDecCells.end()) {
					incDecCount = (findNeighbour->second) + 1;
					findNeighbour->second = incDecCount;
				}
				else {
					incDecCells.insert(pair<long, short>(nbourIndex, 1));
				}
			}

		}
		else {
			//a dead cell remained dead
		}

		updateStatCount++;

		numLiveCells = cellCount;

		//END OF iterating over relevant cells
	}

	//reuse iDecCount, reuse nbourIndex, reuse findNeighbour

	findNeighbour = incDecCells.begin();

	//cout<<"the cells to increment and decrement are"<<endl;

	if (hitBorder) {

	}
	else {
		while (findNeighbour != incDecCells.end()) {
			//cell to be inc/dec
			nbourIndex = findNeighbour->first;
			//what to inc/dec count value by
			incDecCount = findNeighbour->second;

			updateStatCount = relevantCells.find(nbourIndex);

			//if the cell to be inc/dec is in the map
			if (updateStatCount != relevantCells.end()) {
				//cout<<nbourIndex;
				statCount = updateStatCount->second;
				count = statCount / 10;
				//cout<<", b4edit: SC:"<<statCount<< ", c:" <<count;
				//cout<< ", s:";
				state = statCount % 10;
				count = count + incDecCount;
				//cout<< state<< "incdec:" <<incDecCount<< ", editd count:" <<count;
				statCount = count * 10 + state;
				//cout<< "statcount:" <<statCount<<endl;
				updateStatCount->second = statCount;
			}

			else {
				statCount = incDecCount * 10;
				relevantCells.insert(pair<long, short>(nbourIndex, statCount));
			}

			findNeighbour++;
		}
	}

	incDecCells.clear();

}



bool DetectFalsePositive::getFalsePosInfo() {

	return falsePositive;

}

void DetectFalsePositive::printNotFP() {

	ofstream outfile("newObj.txt", ios::app);

	vector<vector<short>>::iterator printIndices = shiftedAddresses.begin();

	vector<short> indexHolder(3);

	while (printIndices != shiftedAddresses.end()) {
		indexHolder = *printIndices;
		cout << "(";
		cout << indexHolder[0] << ", ";
		cout << indexHolder[1] << ", ";
		cout << indexHolder[2];
		cout << ")";

		outfile << "(";
		outfile << indexHolder[0] << ", ";
		outfile << indexHolder[1] << ", ";
		outfile << indexHolder[2];
		outfile << ")";

		printIndices++;
	}
	cout << endl;
	outfile << endl;

}

vector<vector<short>> DetectFalsePositive::getIndices() {
	return shiftedAddresses;
}