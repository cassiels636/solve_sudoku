//
//  board.h
//  Project 4b
//
//  Created by Sara Davila and Cassandra Smith on 3/27/16.
//

#pragma once
#include <iostream>
#include <limits.h>
#include "d_matrix.h"
#include "d_except.h"
#include <list>
#include <fstream>

using namespace std;

typedef int ValueType; // The type of the value in a cell
const int Blank = -1;  // Indicates that a cell is blank

const int SquareSize = 3;  //  The number of cells in a small square
						   //  (usually 3).  The board has
						   //  SquareSize^2 rows and SquareSize^2
						   //  columns.

const int BoardSize = SquareSize * SquareSize;

const int MinValue = 1;
const int MaxValue = 9;

int numSolutions = 0;

class board
	// Stores the entire Sudoku board
{
public:
	board(int);
	void clear();
	void initialize(ifstream &fin);
	void print();
	void printConflicts();
	bool isBlank(int, int);
	bool checkConflicts(int, int, int);
	ValueType getCell(int, int);
	void setCell(int, int, int);
	void updateConflicts();
	void reset(int, int);
	bool isSolved();
	int firstCellRow();
	int firstCellColumn();
	void SolvePuzzle();
	int Recursions;

private:

	// The following matrices go from 1 to BoardSize in each
	// dimension, i.e., they are each (BoardSize+1) * (BoardSize+1)

	matrix<ValueType> value;
	matrix<int> RowConflicts;
	matrix<int> ColConflicts;
	matrix<int> SqConflicts;
	matrix <vector <int> > conflicts;
	int done;
};

board::board(int sqSize)
	: value(BoardSize + 1, BoardSize + 1), RowConflicts(BoardSize + 1, BoardSize + 1, 0), ColConflicts(BoardSize + 1, BoardSize + 1, 0), SqConflicts(BoardSize + 1, BoardSize + 1, 0), conflicts(BoardSize + 1, BoardSize + 1)
	// Board constructor
{
	Recursions = 0;
	done = 0;
	clear();
	for (int i = 1; i <= BoardSize; i++)
		for (int j = 1; j <= BoardSize; j++)
		{
			conflicts[i][j].resize(10);
			for (int k = 1; k <= BoardSize; k++) {
				conflicts[i][j][k] = 0;
			}
		}
	
}

void board::clear()
// Mark all possible values as legal for each board entry
{
	for (int i = 1; i <= BoardSize; i++)
		for (int j = 1; j <= BoardSize; j++)
		{
			value[i][j] = Blank;
		}
}

void board::initialize(ifstream &fin)
// Read a Sudoku board from the input file.
{
	char ch;
	Recursions = 0;;

	for (int i = 1; i <= BoardSize; i++)
		for (int j = 1; j <= BoardSize; j++)
		{
			fin >> ch;

			// If the read char is not Blank
			if (ch != '.')
				setCell(i, j, ch - '0');   // Convert char to int
		}
}

int squareNumber(int i, int j)
// Return the square number of cell i,j (counting from left to right,
// top to bottom.  Note that i and j each go from 1 to BoardSize
{
	// Note that (int) i/SquareSize and (int) j/SquareSize are the x-y
	// coordinates of the square that i,j is in.  

	return SquareSize * ((i - 1) / SquareSize) + (j - 1) / SquareSize + 1;
}

ostream &operator<<(ostream &ostr, vector<int> &v)
// Overloaded output operator for vector class.
{
	for (int i = 1; i < v.size(); i++)
		ostr << v[i] << " ";
	cout << endl;
	return ostr;
}

ValueType board::getCell(int i, int j)
// Returns the value stored in a cell.  Throws an exception
// if bad values are passed.
{
	if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
		return value[i][j];
	else
		throw rangeError("bad value in getCell");
}

void board::setCell(int i, int j, int Num) {
	value[i][j] = Num;
	updateConflicts();
}

bool board::isBlank(int i, int j)
// Returns true if cell i,j is blank, and false otherwise.
{
	if (i < 1 || i > BoardSize || j < 1 || j > BoardSize)
		throw rangeError("bad value in setCell");

	return (getCell(i, j) == Blank);
}

bool board::checkConflicts(int i, int j, int Num) {
	return (conflicts[i][j][Num] == 0);
}

void board::print()
// Prints the current board.
{
	for (int i = 1; i <= BoardSize; i++)
	{
		if ((i - 1) % SquareSize == 0)
		{
			cout << " -";
			for (int j = 1; j <= BoardSize; j++)
				cout << "---";
			cout << "-";
			cout << endl;
		}
		for (int j = 1; j <= BoardSize; j++)
		{
			if ((j - 1) % SquareSize == 0)
				cout << "|";
			if (!isBlank(i, j))
				cout << " " << getCell(i, j) << " ";
			else
				cout << "   ";
		}
		cout << "|";
		cout << endl;
	}

	cout << " -";
	for (int j = 1; j <= BoardSize; j++)
		cout << "---";
	cout << "-";
	cout << endl;
}

void board::printConflicts() {
	for (int i = 1; i <= BoardSize; i++) {
		for (int j = 1; j <= BoardSize; j++) {
			cout << "Cell (row = " << i << ", column = " << j << ") Conflicts: " << conflicts[i][j] << endl;
		}
	}
}

void board::updateConflicts() {

	for (int i = 1; i <= BoardSize; i++) {
		for (int j = 1; j <= BoardSize; j++) {
			if (isBlank(i, j)== false) {
	
				RowConflicts[i][value[i][j]] = 1;
				ColConflicts[j][value[i][j]] = 1;
				SqConflicts[squareNumber(i, j)][value[i][j]] = 1;
			}

			for (int k = 1; k <= BoardSize; k++) {
				conflicts[i][j][k] = RowConflicts[i][k] + ColConflicts[j][k] + SqConflicts[squareNumber(i, j)][k];
			}
		}
	}

}

void board::reset(int i ,int j) {
	int temp = 0;

	temp = value[i][j];
	value[i][j] = Blank;
	RowConflicts[i][temp] = 0;
	ColConflicts[j][temp] = 0;
	SqConflicts[squareNumber(i, j)][temp] = 0;

	for (int x = 1; x <= BoardSize; x++)
		for (int y = 1; y <= BoardSize; y++) {
			for (int k = 1; k <= BoardSize; k++) {
				conflicts[x][y][k] = RowConflicts[x][k] + ColConflicts[y][k] + SqConflicts[squareNumber(x, y)][k];
			}
		}
}

bool board::isSolved(){
    int i, j;
    
    for (i = 1; i <= BoardSize; i++) {
        for (j = 1; j <= BoardSize; j++) {
				if ((conflicts[i][j][getCell(i,j)] != 3) || (isBlank(i, j) == true)) {
					return false;
				}
        }
    }
    
    return true;
}

int SquareToCellRow(int Sq, int n) {
	int row;

	if (Sq <= 3)
		row = ((n - 1) / 3) + 1;
	else if (Sq > 3 && Sq <= 6)
		row = ((n - 1) / 3) + 4;
	else if (Sq >= 7)
		row = ((n - 1) / 3) + 7;

	return row;
}

int SquareToCellCol(int Sq, int n) {
	int col;
	if (Sq == 1 || Sq == 4 || Sq == 7) {
		if (n <= 3)
			col = n;
		else if (n>3 && n <= 6)
			col = n - 3;
		else if (n >= 7)
			col = n - 6;
	}
	else if (Sq == 2 || Sq == 5 || Sq == 8) {
		if (n <= 3)
			col = n + 3;
		else if (n>3 && n <= 6)
			col = n;
		else if (n >= 7)
			col = n - 3;
	}
	else if (Sq == 3 || Sq == 6 || Sq == 9) {
		if (n <= 3)
			col = n + 6;
		else if (n>3 && n <= 6)
			col = n + 3;
		else if (n >= 7)
			col = n;
	}

	return col;
}

void board::SolvePuzzle() {
	int x, y;
	Recursions++;
	if (isSolved()) {
		cout << "Finished!" << endl;
		print();
		done = 1;
		return;
	}
	else {
		int i;
		x = firstCellRow();
		y = firstCellColumn();/*
		//////////////////////////////////////////////
		for (int l = 1; l <= BoardSize; l++) {  //value
			for (int m = 1; m <= BoardSize; m++) {	//square
				for (int n = 1; n <= BoardSize; n++) {	//square cel
					p = SquareToCellRow(m, n);
					q = SquareToCellCol(m, n);
					if (isBlank(p, q) && checkConflicts(p, q, l)) {
						setCell(p, q, l);
						SolvePuzzle();
						if (done == 1)
							return;
						else
							reset(p, q);
					}
				}
			}
		}
		/////////////////////////////////////////////////*/
		for (i = 1; i <= 9; i++) {
			if (checkConflicts(x, y, i)) {
				setCell(x, y, i);
				SolvePuzzle();
				if (done == 1)
					return;
				else
					reset(x, y);
			}
		}
		return;
	}
}

int board::firstCellRow() {
	int temp = 0, row, leastZeros = BoardSize;

	for (int i = 1; i <= BoardSize; i++) {
		for (int j = 1; j <= BoardSize; j++) {
			if (isBlank(i, j)) {
				for (int k = 1; k <= BoardSize; k++) {
					if (conflicts[i][j][k] == 0) {
						temp++;
					}
				}
				if (temp < leastZeros) {
					leastZeros = temp;
					row = i;
				}
			}
		}
	}
	return row;
}

int board::firstCellColumn() {
	int temp = 0, col, leastZeros = 9;
	for (int i = 1; i <= BoardSize; i++) {
		for (int j = 1; j <= BoardSize; j++) {
			if (isBlank(i, j)) {
				for (int k = 1; k <= BoardSize; k++) {
					if (conflicts[i][j][k] == 0) {
						temp++;
					}
				}
				if (temp < leastZeros) {
					leastZeros = temp;
					col = j;
				}
			}
		}
	}
	return col;
}
