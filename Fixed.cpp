// Fixed.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "LookupNeighbours.h"
#include "RunXSimulations.h"

int main()
{
	int numExperiments = 0;

	cout << endl << "Enter the number of experiments: ";

	cin >> numExperiments;

	cout << endl;

	RunXSimulations runXExperiments = RunXSimulations(numExperiments);

	return 0;
}


