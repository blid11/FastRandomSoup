#pragma once

#pragma once

#include <unordered_map>
#include <vector>

using namespace std;

class LrgLookup {

public:

	//ATTRIBUTES
	unordered_map<long, long*> lookupTable;
	long*** binaryResultStorage;
	int* copyToArr;


	//FUNCTIONS
	//Constructor
	LrgLookup();
	void fillLookup();
	bool checkIfBorderCell(int i, int j, int k);
	void calculateBinary(int convertThis);
	long getAddressEncoded(const int& x, const int& y, const int& z);
	long* getNeighbours(long indices);

};


LrgLookup::LrgLookup() {

	lookupTable.reserve(421875);
	int copyToArray[7] = { 0,0,0,0,0,0,0 };
	copyToArr = copyToArray;

	binaryResultStorage = new long** [75];
	for (int i = 0; i < 75; i++) {
		binaryResultStorage[i] = new long* [75];
		for (int j = 0; j < 75; j++) {
			binaryResultStorage[i][j] = new long[75];
			for (int k = 0; k < 75; k++) {
				binaryResultStorage[i][j][k] = 0;
			}
		}
	}

	fillLookup();

}

void LrgLookup::fillLookup() {
	int indexIArray[7] = { 0, 0, 0, 0, 0, 0, 0 };
	int indexJArray[7] = { 0, 0, 0, 0, 0, 0, 0 };
	int indexKArray[7] = { 0, 0, 0, 0, 0, 0, 0 };

	int indexEncoded[21];
	for (int b = 0; b < 21; b++) {
		indexEncoded[b] = 0;
	}

	for (int i = 0; i < 75; i++) {
		for (int j = 0; j < 75; j++) {
			for (int k = 0; k < 75; k++) {
				//filling the unordered map first with the unsigned int encoding indices
				//what is i in binary
				//put this in an int array
				calculateBinary(i);
				for (int e = 0; e < 7; e++) {
					indexIArray[e] = copyToArr[e];
				}

				//what is j in binary
				calculateBinary(j);
				for (int e = 0; e < 7; e++) {
					indexJArray[e] = copyToArr[e];
				}

				//what is k in binary
				calculateBinary(k);
				for (int e = 0; e < 7; e++) {
					indexKArray[e] = copyToArr[e];
				}
				//place the binary representations for i, j and k side by side in an array
				//convert this entire array to a single decimal number
				//place the result in an unsigned int that will have a value 0 to ~65000.

				indexEncoded[0] = indexIArray[0] * 1048576;
				indexEncoded[1] = indexIArray[1] * 524288;
				indexEncoded[2] = indexIArray[2] * 262144;
				indexEncoded[3] = indexIArray[3] * 131072;
				indexEncoded[4] = indexIArray[4] * 65536;
				indexEncoded[5] = indexIArray[5] * 32678;
				indexEncoded[6] = indexIArray[6] * 16384;

				indexEncoded[7] = indexJArray[0] * 8192;
				indexEncoded[8] = indexJArray[1] * 4096;
				indexEncoded[9] = indexJArray[2] * 2048;
				indexEncoded[10] = indexJArray[3] * 1024;
				indexEncoded[11] = indexJArray[4] * 512;
				indexEncoded[12] = indexJArray[5] * 256;
				indexEncoded[13] = indexJArray[6] * 128;

				indexEncoded[14] = indexKArray[0] * 64;
				indexEncoded[15] = indexKArray[1] * 32;
				indexEncoded[16] = indexKArray[2] * 16;
				indexEncoded[17] = indexKArray[3] * 8;
				indexEncoded[18] = indexKArray[4] * 4;
				indexEncoded[19] = indexKArray[5] * 2;
				indexEncoded[20] = indexKArray[6] * 1;

				long encoded = 0;

				for (int e = 0; e < 21; e++) {
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

	long primaryCellEncoded = 0;

	for (int i = 0; i < 75; i++) {
		for (int j = 0; j < 75; j++) {
			for (int k = 0; k < 75; k++) {

				primaryCellEncoded = binaryResultStorage[i][j][k];

				//calculate the neighbours for a given cell using indices as ints
				//then look up the precalculated int that encodes the indices
				//convert this int to an unsigned int and place it in the neighbourUnInt array

				long* pointsToNeighbours = new long[26];
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
								else if (l < 0 || m < 0 || o < 0 || l > 74 || m > 74 || o > 74) {
									//neighbours that are out of bounds are given the value 32767 bc this 
									//value is stored in a negative form in the lookup table 
									pointsToNeighbours[c] = 1238475;
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


bool LrgLookup::checkIfBorderCell(int i, int j, int k) {

	bool isBorderCell = false;

	bool iIsOnBorder = ((i == 0) || (i == 1) || (i == 2) || (i == 74) || (i == 73) || (i == 72));

	bool jIsOnBorder = ((j == 0) || (j == 1) || (j == 2) || (j == 74) || (j == 73) || (i == 72));

	bool kIsOnBorder = ((k == 0) || (k == 1) || (k == 2) || (k == 74) || (k == 73) || (i == 72));

	if ((iIsOnBorder || jIsOnBorder || kIsOnBorder)) {
		isBorderCell = true;
	}
	else {
		isBorderCell = false;
	}

	return isBorderCell;

}


//will concatenate the 3 c strings, and

void LrgLookup::calculateBinary(int convertThis) {

	//this function will take an int, and put its binary representation in a cstring
	//the char array is set to {0,0,0,0,0} to start
	//the first value is placed in the last spot of the array

	int convert = convertThis;
	int remainder = 0;
	int quotient = 0;

	int counter = 6;

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

long LrgLookup::getAddressEncoded(const int& x, const int& y, const int& z) {

	long returnThis = binaryResultStorage[x][y][z];

	return returnThis;

}

long* LrgLookup::getNeighbours(long indices) {

	unordered_map<long, long*>::iterator iter = lookupTable.find(indices);

	long* returnThis = nullptr;

	if (iter != lookupTable.end()) {
		returnThis = iter->second;
		return returnThis;
	}
	else if (indices != 1238475) {
		indices = -1 * indices;
		iter = lookupTable.find(indices);
		returnThis = iter->second;
		return returnThis;
	}
	else {
		return returnThis;
	}

}


