//
//  main.cpp
//  Project 4b
//
//  Created by Sara Davila and Cassandra Smith on 3/27/16.
//

#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include "board.h"

using namespace std;

int main()
{
	ifstream fin;

	// Read the sample grid from the file.
	string fileName = "sudoku.txt";

	fin.open(fileName.c_str());
	if (!fin)
	{
		cerr << "Cannot open " << fileName << endl;
		exit(1);
	}

	try
	{
		
		int AvgRecursions, TotalRecursions = 0, Puzzles = 0;

		while (fin && fin.peek() != 'Z')
		{
			board b1(SquareSize);
			Puzzles++;
			cout << "PUZZLE " << Puzzles << endl;
			b1.initialize(fin);
			b1.print();
			b1.SolvePuzzle();
			cout << "Recursions:  " << b1.Recursions << endl << endl;
			//cout << "Please press enter. " << endl;
			//cin.get();
			TotalRecursions = TotalRecursions + b1.Recursions;
		}

		AvgRecursions = TotalRecursions / Puzzles;
		cout << "Average Number of Recursions: " << AvgRecursions << endl;


	}
	catch (indexRangeError &ex)
	{
		cout << ex.what() << endl;
		exit(1);
	}

	cin.get();
}