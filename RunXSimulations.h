#pragma once


#ifndef RunXSimulations_h
#define RunXSimulations_h


#endif /* RunXSimulations_h */


#include <vector>
#include <iostream>
#include <random>
#include <bitset>
#include <algorithm>
#include <fstream>
#include "LookupNeighbours.h"
#include "LrgLookup.h"
#include "FindSignature.h"

template< size_t size>
typename std::bitset<size> random_bitset(double p = 0.5) {

	typename std::bitset<size> bits;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::bernoulli_distribution d(p);

	for (int n = 0; n < size; ++n) {
		bits[n] = d(gen);
	}

	return bits;
}


class RunXSimulations {

public:
	//ATTRIBUTES
	int noOfCentreCells;
	short gridDimension;

	int numExpToRun;
	short cellThatHitBorder;

	int gliderV;
	int gliderE;
	int newObjects;
	int falsePositives;

	short* updateTable;
	short* beginOfTable;

	LookupNeighbours* lookUpTable;

	//relevant for the simulation
	int numLiveCells;
	bool hitBorder;
	bool checkBorderList;
	bool stillLife;
	vector<short> liveCellCurrent;
	vector<short> liveCellPrev;

	vector<short> addressToEdit;
	vector<short> editBy;

	vector<short> borderList;

	//Functions
	RunXSimulations(int numExperiments);
	void runExperiments();
	void makeNewSim();
	void clearUpdateTable();
	void clearIncDecVectors(int numEdited);
	void performUpdate();
	void printFinalTally();

};

RunXSimulations::RunXSimulations(int numExperiments) {

	numExpToRun = numExperiments;

	noOfCentreCells = 729;

	lookUpTable = new LookupNeighbours;

	updateTable = new short[32768];
	beginOfTable = updateTable;

	cellThatHitBorder = 0;
	gliderV = 0;
	gliderE = 0;
	newObjects = 0;
	falsePositives = 0;

	liveCellCurrent.reserve(250);
	liveCellPrev.reserve(250);

	addressToEdit = vector<short>(6500, 0);
	editBy = vector<short>(6500, 0);
	borderList.reserve(200);

	stillLife = false;
	hitBorder = false;
	checkBorderList = false;

	//populate the eelements of the table with 0s before using it
	clearUpdateTable();

	runExperiments();

	printFinalTally();

}




void RunXSimulations::runExperiments() {

	ofstream outfile("newObj.txt", ios::app);

	LrgLookup* lrgLookU = new LrgLookup();

	int simulationNum = 0;

	while (simulationNum < numExpToRun) {

		//cout<<"Running experiment: "<<simulationNum<<endl;


		if (((simulationNum + 1) > 1999) && ((simulationNum + 1) % 2000 == 0)) {
			cout << "Running experiment " << (simulationNum + 1) << endl;
		}
		else {
		}

		stillLife = false;
		hitBorder = false;
		checkBorderList = false;

		bool oscillator = false;
		int genNumber = 0;

		short whatHitBorder = 0;
		bool falseP = false;

		makeNewSim();

		while ((!hitBorder) && numLiveCells && (!oscillator) && (!stillLife)) {

			//cout<<"updating generation"<<genNumber<<endl;

			performUpdate();
			genNumber++;

			if (hitBorder) {

				FindSignature findThisSignature = FindSignature(liveCellCurrent, cellThatHitBorder, gridDimension, lookUpTable, lrgLookU);

				whatHitBorder = findThisSignature.getSignatureNumber();

				if (whatHitBorder == 0) {
					gliderV++;

					/*
					falseP = findThisSignature.getFalsePos();
					if(falseP){
						cout<<"there was a false positive"<<endl;
						falsePositives++;
					}
					else{
						cout<<"No false positive, glider hit wall, success"<<endl;
						outfile<<"No false positive, glider hit wall, success"<<endl;
						short printThis = 0;
						cout<<"Live cells CURRENT are:"<<endl;
						outfile<<"Live cells CURRENT are:"<<endl;
						for(vector<short>::iterator iterLive = liveCellCurrent.begin(); iterLive != liveCellCurrent.end(); iterLive++){
							printThis = *iterLive;
							cout<<printThis<<endl;
							outfile<<printThis<<endl;
							}
						cout<<endl;
						outfile<<endl;
						//could put option to print here
					}
				*/

				}
				else if (whatHitBorder == 1) {
					gliderE++;
				}
				else {
					newObjects++;

					falseP = findThisSignature.getFalsePos();
					if (falseP) {
						cout << "there was a false positive" << endl;
						falsePositives++;
					}
					else {
						cout << "No false positive, glider hit wall, success" << endl;
						outfile << "No false positive, glider hit wall, success" << endl;
						cout << "exp no. " << simulationNum << endl;
						outfile << "exp no. " << simulationNum << endl;
						short printThis = 0;
						cout << "Live cells CURRENT are:" << endl;
						outfile << "Live cells CURRENT are:" << endl;
						for (vector<short>::iterator iterLive = liveCellCurrent.begin(); iterLive != liveCellCurrent.end(); iterLive++) {
							printThis = *iterLive;
							cout << printThis << endl;
							outfile << printThis << endl;
						}
						cout << endl;
						outfile << endl;
						//could put option to print here
					}

				}

			}
			else if (genNumber > 75) {
				oscillator = true;
			}
			else if (liveCellCurrent == liveCellPrev) {
				stillLife = true;
				//cout<<"end condition still life"<<endl;
			}
			else {
				//oscillator = false;
			}

			/*
			short printThis = 0;
			cout<<"Live cells CURRENT are:"<<endl;
			for(vector<short>::iterator iterLive = liveCellCurrent.begin(); iterLive != liveCellCurrent.end(); iterLive++){
				printThis = *iterLive;
				cout<<printThis<<endl;
				}
			cout<<endl;
			 */

			liveCellPrev = liveCellCurrent;

			liveCellCurrent.clear();

		}

		//END OF 1 Simulation
		liveCellPrev.clear();

		//relevantCells.clear();
		clearUpdateTable();



		if (checkBorderList) {
			borderList.clear();
		}
		else {
		}

		simulationNum++;
	}

	delete lrgLookU;

}

void RunXSimulations::makeNewSim() {

	numLiveCells = 0;

	short indexToAdd = 0;

	int bottomLeftCorner = 11;

	int topRightCorner = 19;

	std::bitset<729> b = random_bitset<729>(0.25);

	size_t placeInArray = 0;

	short state = 0;

	for (int i = bottomLeftCorner; i < topRightCorner; i++) {
		for (int j = bottomLeftCorner; j < topRightCorner; j++) {
			for (int k = bottomLeftCorner; k < topRightCorner; k++) {

				//first decide if the cell is live or dead with liveDead()

				state = b[placeInArray];
				placeInArray++;

				if (state == 0) {
					continue;
					//skip to the next iteration of the loop
				}
				else {
					//carry out the following steps
				}


				indexToAdd = lookUpTable->getAddressEncoded(i, j, k);

				liveCellPrev.push_back(indexToAdd);

				//step 1: check if this value has already been modified

				if (updateTable[indexToAdd] != 0) {

					//get the neighbour and state value for this cell
					//the state should now be set to on, its neighbour count does not increase
					//as whatever live cells have added it as a neighbour have incremented its value
					short editNStat = updateTable[indexToAdd];
					//this ensures you don't set it to 1 more than once aka = 2
					editNStat = (editNStat / 10) * 10 + 1;
					updateTable[indexToAdd] = editNStat;

				}
				else {
					//the 1 means state 1
					short addNStat = 1;
					updateTable[indexToAdd] = addNStat;

				}

				numLiveCells++;

				short* pointsToNeighbours = lookUpTable->getNeighbours(indexToAdd);

				short editNCount = 0;
				short editState = 0;

				for (int neighbo = 0; neighbo < 26; neighbo++) {

					short nToAdd = pointsToNeighbours[neighbo];

					if (updateTable[nToAdd] != 0) {
						//it is either present as a neighbour or a live cell
						editNCount = updateTable[nToAdd] / 10;

						//

						editState = (updateTable[nToAdd]) % 10;
						//preserve info about state and neighbour count but increment n count
						editNCount = (editNCount + 1) * 10 + editState;

						updateTable[nToAdd] = editNCount;

					}
					else {
						//give it a neighbour count of 1 and state 0 represented by 10
						short addNStat = 10;
						updateTable[nToAdd] = addNStat;
						//cout<<nToAdd<<": new :" << neighbo <<endl;

					}

				}

			}
		}
	}




	/*
	vector<int> cell0 = { 10, 10, 10 };
	vector<int> cell1 = { 11, 10, 10 };
	vector<int> cell2 = { 10, 11, 10 };
	vector<int> cell3 = { 11, 11, 10 };
	vector<int> cell4 = { 10, 9, 11 };
	vector<int> cell5 = { 11, 9, 11 };
	vector<int> cell6 = { 10, 11, 11 };
	vector<int> cell7 = { 11, 11, 11 };
	vector<int> cell8 = { 10, 11, 12 };
	vector<int> cell9 = { 11, 11, 12 };


	randomCellList = { cell0, cell1, cell2, cell3, cell4, cell5, cell6, cell7, cell8, cell9 };
	*/



}


void RunXSimulations::performUpdate() {

	//now the map has been filled for the first generation
	//print out all the values
	//unordered_map<short, short>::iterator updateStatCount = relevantCells.begin();

	int index = 0;
	int numEdited = 0;
	short statCount = 0;
	short state = 0;
	short count = 0;

	short nbourIndex = 0;

	short incDecCount = 0;

	int cellCount = 0;

	while (index < 32768) {

		statCount = updateTable[index];
		//cout << "cell " << index << ": state" <<statCount<< endl;

		if (statCount == 0) {
			//cout << index << endl;
			index++;
		}

		else {
			count = statCount / 10;
			state = statCount % 10;

			if (state == 0 && count != 6) {
				//a dead cell remains dead
				index++;
			}
			//a live cell is more likely to remain alive than to die.. i think
			else if (state && ((count == 5) || (count == 6) || (count == 7))) {
				liveCellCurrent.push_back(index);
				cellCount++;
				//cout<<index<<"lived, state: "<< state << ", nCount: "<<count<<endl;
				index++;
			}
			else if (state) {
				//first change the state of the cell itself to 0
				statCount = count * 10;
				updateTable[index] = statCount;
				//cout<<index<<"died, state: "<< state << ", nCount: "<<count<< ", statCount"<<updateTable[index]<< endl;

				//the cell died and the nCount for its neighbours must be decremented to 0
				short* pointsToNbours = lookUpTable->getNeighbours(index);
				//AS THE CELL WAS LIVE, ALL NEIGHBOURS SHOULD BE IN MAP

				if (pointsToNbours != NULL) {
					for (int nbour = 0; nbour < 26; nbour++) {
						nbourIndex = pointsToNbours[nbour];
						//neighbour index iterator points to the map of cells to increment and decrement
						if (nbourIndex != 32767) {
							addressToEdit[numEdited] = nbourIndex;
							editBy[numEdited] = -1;
							numEdited++;
						}
						else {
							continue;
						}
					}
				}
				else {
					hitBorder = true;
				}

				index++;

			}
			//the cell is dead but count = 6
			else {

				if (checkBorderList) {
					if ((find(borderList.begin(), borderList.end(), index)) != borderList.end()) {
						hitBorder = true;
						short neg = -1 * index;
						cellThatHitBorder = neg;
						liveCellCurrent.push_back(neg);
						cellCount++;
					}
					else {
						liveCellCurrent.push_back(index);
						cellCount++;
					}

					//change the state to 1
					statCount = count * 10 + 1;
					updateTable[index] = statCount;
					//cout<<index<<"born, state: "<< state << ", nCount: "<<count<< ", statcount" <<statCount<<endl;
					//this works fine

					short* pointsToNbours = lookUpTable->getNeighbours(index);
					//AS THE CELL WAS LIVE, ALL NEIGHBOURS SHOULD BE IN MAP
					if (pointsToNbours != NULL) {
						for (int nbour = 0; nbour < 26; nbour++) {
							nbourIndex = pointsToNbours[nbour];
							//neighbour index iterator points to the map of cells to increment and decrement
							if (nbourIndex != 32767) {
								addressToEdit[numEdited] = nbourIndex;
								editBy[numEdited] = 1;
								numEdited++;
							}
							else {
								continue;
							}
						}
					}
					else {
						hitBorder = true;
					}

				}

				else {
					liveCellCurrent.push_back(index);
					cellCount++;

					//change the state to 1
					statCount = count * 10 + 1;
					updateTable[index] = statCount;
					//cout<<index<<"born, state: "<< state << ", nCount: "<<count<< ", statcount" <<statCount<<endl;
					//this works fine

					short* pointsToNbours = lookUpTable->getNeighbours(index);
					//AS THE CELL WAS LIVE, ALL NEIGHBOURS SHOULD BE IN MAP
					if (pointsToNbours != NULL) {
						for (int nbour = 0; nbour < 26; nbour++) {
							nbourIndex = pointsToNbours[nbour];
							//neighbour index iterator points to the map of cells to increment and decrement
							if (nbourIndex != 32767) {
								addressToEdit[numEdited] = nbourIndex;
								editBy[numEdited] = 1;
								numEdited++;
							}
							else {
								continue;
							}
						}
					}
					else {
						hitBorder = true;
					}

				}

				index++;

			}

		}

		numLiveCells = cellCount;

		//END OF iterating over relevant cells
	}

	//reuse iDecCount, reuse nbourIndex, reuse findNeighbour

	vector<short>::iterator indToEdit = addressToEdit.begin();
	vector<short>::iterator editByThis = editBy.begin();
	//cout<<"the cells to increment and decrement are"<<endl;

	//cout<<endl<< "the number of live cells is: "<< numLiveCells<<endl;

	if (hitBorder) {
		//don't bother incrementing and decrementing
	}
	else {
		while (nbourIndex != 0) {
			//cell to be inc/dec
			nbourIndex = *indToEdit;
			//what to inc/dec count value by
			incDecCount = *editByThis;

			if (nbourIndex <= 0) {
				nbourIndex = -1 * nbourIndex;
				borderList.push_back(nbourIndex);
				checkBorderList = true;
			}
			else {
				//proceed as usual
			}
			//if the cell to be inc/dec is in the map

			if (updateTable[nbourIndex] != 0) {
				//cout<<nbourIndex;
				statCount = updateTable[nbourIndex];
				count = statCount / 10;
				//cout<<", b4edit: SC:"<<statCount<< ", c:" <<count;
				//cout<< ", s:";
				state = statCount % 10;
				count = count + incDecCount;
				//cout<< state<< "incdec:" <<incDecCount<< ", editd count:" <<count;
				statCount = count * 10 + state;
				//cout<< "statcount:" <<statCount<<endl;
				updateTable[nbourIndex] = statCount;
			}

			else {
				statCount = incDecCount * 10;
				updateTable[nbourIndex] = statCount;
			}

			indToEdit++;
			editByThis++;
		}
	}

	clearIncDecVectors(numEdited);

}

void RunXSimulations::printFinalTally() {

	cout << endl << "the number of gliderV was: " << gliderV << endl;

	cout << endl << "the number of gliderE was: " << gliderE << endl;

	cout << endl << "the number of new objects was: " << newObjects << endl;


	if (falsePositives != newObjects) {
		cout << endl << "There was a success, search success " << endl;
	}
	else {
		cout << endl << "NOTHING REALLY NEW " << endl;
	}

}

void RunXSimulations::clearIncDecVectors(int numEdited) {

	for (int i = 0; i <= numEdited; i++) {
		addressToEdit[i] = 0;
		editBy[i] = 0;

	}

}

void RunXSimulations::clearUpdateTable() {

	for (int i = 0; i < 32768; i++) {
		updateTable[i] = 0;
	}

}

