#pragma once

#ifndef LookupNeighbours_h
#define LookupNeighbours_h


#endif /* LookupNeighbours_h */


#include <unordered_map>
#include <vector>

using namespace std;

class LookupNeighbours {

public:

	//ATTRIBUTES
	unordered_map<short, short*> lookupTable;
	short*** binaryResultStorage;
	short gridDim;
	short* copyToArr;


	//FUNCTIONS
	//Constructor
	LookupNeighbours();
	void fillLookup();
	bool checkIfBorderCell(int i, int j, int k);
	void calculateBinary(int convertThis);
	short getAddressEncoded(const int& x, const int& y, const int& z);
	short* getNeighbours(short indices);

};


LookupNeighbours::LookupNeighbours() {

	lookupTable.reserve(32768);
	short copyToArray[5] = { 0,0,0,0,0 };
	copyToArr = copyToArray;

	binaryResultStorage = new short** [32];
	for (int i = 0; i < 32; i++) {
		binaryResultStorage[i] = new short* [32];
		for (int j = 0; j < 32; j++) {
			binaryResultStorage[i][j] = new short[32];
			for (int k = 0; k < 32; k++) {
				binaryResultStorage[i][j][k] = 0;
			}
		}
	}

	fillLookup();

}

void LookupNeighbours::fillLookup() {
	short indexIArray[5] = { 0, 0, 0, 0, 0 };
	short  indexJArray[5] = { 0, 0, 0, 0, 0 };
	short  indexKArray[5] = { 0, 0, 0, 0, 0 };

	short indexEncoded[16];
	for (int b = 0; b < 16; b++) {
		indexEncoded[b] = 0;
	}

	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			for (int k = 0; k < 32; k++) {
				//filling the unordered map first with the unsigned int encoding indices
				//what is i in binary
				//put this in an int array
				calculateBinary(i);
				for (int e = 0; e < 5; e++) {
					indexIArray[e] = copyToArr[e];
				}

				//what is j in binary
				calculateBinary(j);
				for (int e = 0; e < 5; e++) {
					indexJArray[e] = copyToArr[e];
				}

				//what is k in binary
				calculateBinary(k);
				for (int e = 0; e < 5; e++) {
					indexKArray[e] = copyToArr[e];
				}
				//place the binary representations for i, j and k side by side in an array
				//convert this entire array to a single decimal number
				//place the result in an unsigned int that will have a value 0 to ~65000.

				indexEncoded[0] = 0;

				indexEncoded[1] = indexIArray[0] * 16384;
				indexEncoded[2] = indexIArray[1] * 8192;
				indexEncoded[3] = indexIArray[2] * 4096;
				indexEncoded[4] = indexIArray[3] * 2048;
				indexEncoded[5] = indexIArray[4] * 1024;

				indexEncoded[6] = indexJArray[0] * 512;
				indexEncoded[7] = indexJArray[1] * 256;
				indexEncoded[8] = indexJArray[2] * 128;
				indexEncoded[9] = indexJArray[3] * 64;
				indexEncoded[10] = indexJArray[4] * 32;

				indexEncoded[11] = indexKArray[0] * 16;
				indexEncoded[12] = indexKArray[1] * 8;
				indexEncoded[13] = indexKArray[2] * 4;
				indexEncoded[14] = indexKArray[3] * 2;
				indexEncoded[15] = indexKArray[4] * 1;

				short encoded = 0;

				for (int e = 0; e < 16; e++) {
					encoded += indexEncoded[e];
				}

				bool borderCell = checkIfBorderCell(i, j, k);

				if (borderCell) {
					binaryResultStorage[i][j][k] = -encoded;
				}
				else {
					binaryResultStorage[i][j][k] = encoded;
				}

			}
		}
	}

	//by setting these values, neighbour cells beyopnd the boundary cannot be calculated
	//this isn't an issue as the simulation will be stopped before the neighbour values for an edge cell can be requested

	short primaryCellEncoded = 0;

	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			for (int k = 0; k < 32; k++) {

				primaryCellEncoded = binaryResultStorage[i][j][k];

				//calculate the neighbours for a given cell using indices as ints
				//then look up the precalculated int that encodes the indices
				//convert this int to an unsigned int and place it in the neighbourUnInt array

				short* pointsToNeighbours = new short[26];
				for (int e = 0; e < 26; e++) {
					pointsToNeighbours[e] = 0;
				}

				int c = 0;
				while (c < 26) {
					for (int l = (i - 1); l <= (i + 1); l++) {
						for (int m = (j - 1); m <= (j + 1); m++) {
							for (int o = (k - 1); o <= (k + 1); o++) {
								if ((l == i) && (m == j) && (o == k)) {
									continue;
								}
								else if (l < 0 || m < 0 || o < 0 || l > 31 || m > 31 || o > 31) {
									//neighbours that are out of bounds are given the value 32767 bc this 
									//value is stored in a negative form in the lookup table 
									pointsToNeighbours[c] = 32767;
									c++;
								}
								else {
									pointsToNeighbours[c] = binaryResultStorage[l][m][o];
									c++;
								}
							}
						}
					}
				}

				lookupTable.insert({ primaryCellEncoded, pointsToNeighbours });

				//look up the precalculated int for the cell in question and convert it to unsign
				//insert the unsign int and the unsigned int array in the unordered map


			}
		}
	}


}


bool LookupNeighbours::checkIfBorderCell(int i, int j, int k) {

	bool isBorderCell = false;

	bool iIsOnBorder = ((i == 0) || (i == 1) || (i == 2) || (i == (gridDim - 1)) || (i == (gridDim - 2)) || (i == (gridDim - 3)));

	bool jIsOnBorder = ((j == 0) || (j == 1) || (j == 2) || (j == (gridDim - 1)) || (j == (gridDim - 2)) || (i == (gridDim - 3)));

	bool kIsOnBorder = ((k == 0) || (k == 1) || (k == 2) || (k == (gridDim - 1)) || (k == (gridDim - 2)) || (i == (gridDim - 3)));

	if ((iIsOnBorder || jIsOnBorder || kIsOnBorder)) {
		isBorderCell = true;
	}
	else {
		isBorderCell = false;
	}

	return isBorderCell;

}


//will concatenate the 3 c strings, and

void LookupNeighbours::calculateBinary(int convertThis) {

	//this function will take an int, and put its binary representation in a cstring
	//the char array is set to {0,0,0,0,0} to start
	//the first value is placed in the last spot of the array

	int convert = convertThis;
	unsigned short remainder = 0;
	int quotient = 0;

	int counter = 4;

	//by filling all 5 positions no matter what, starting with the rightmost
	//will fill the 5th position with a 0 even if the quotient and remainder have already been 0
	//this will be allow for easy conversion back to decimal later.
	while (counter != -1) {

		quotient = convert / 2;
		remainder = convert % 2;
		convert = quotient;

		//the last position has the last part of the binary rep placed in it
		copyToArr[counter] = remainder;

		counter--;

	}

}

short LookupNeighbours::getAddressEncoded(const int& x, const int& y, const int& z) {

	short returnThis = binaryResultStorage[x][y][z];

	return returnThis;

}

short* LookupNeighbours::getNeighbours(short indices) {

	unordered_map<short, short*>::iterator iter = lookupTable.find(indices);

	short* returnThis = nullptr;

	if (iter != lookupTable.end()) {
		returnThis = iter->second;
		return returnThis;
	}
	else if (indices != 32767) {
		indices = -1 * indices;
		iter = lookupTable.find(indices);
		returnThis = iter->second;
		return returnThis;
	}
	else {
		return returnThis;
	}

}

