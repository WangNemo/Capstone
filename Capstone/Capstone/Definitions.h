#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include "Signal.h"

#define MAX(x,y)     ( ( x ) > ( y ) ? ( x ) : ( y ) )
#define MIN(x,y)     ( ( x ) < ( y ) ? ( x ) : ( y ) )

#define PI 3.1415926535897932384626433832795
#define print std::cout<<
#define end <<std::endl


double maxInArray(double* darray, int size); 

double minInArray(double* darray, int size); 

double avgInArray(double* darray, int size); 


struct Complex {
	double real = 0;
	double imaginary = 0;

	Complex(int r, int i) {
		real = r;
		imaginary = i;
	}

	Complex() {}
};

class doubleGrid {
public:
	int const ROWS, COLUMNS;
	double** grid;
	double operator()(int row, int col) {
		if (row >= ROWS || col >= COLUMNS)
			return 0;
		else return grid[row][col];
	}

	doubleGrid(double** grid, int rows, int cols) : grid(grid), ROWS(rows), COLUMNS(cols) {
	}

	void doubleGrid::negate() {
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLUMNS; j++) {
				grid[i][j] = -grid[i][j];
			}
		}
	}

	~doubleGrid(){
		for (int i = 0; i < ROWS; i++) {
			delete grid[i];
		}
	}
};

class boolGrid {
public:
	int const ROWS, COLUMNS;
	bool** grid;
	bool operator()(int row, int col) {
		return grid[row][col];
	}

	boolGrid(bool** grid, int rows, int cols) : grid(grid), ROWS(rows), COLUMNS(cols) {

	}

	~boolGrid(){
		for (int i = 0; i < ROWS; i++) {
			delete grid[i];
		}
	}
};

double* mexFunction(double* x, int nsamples, int cf, int fs);

namespace staticTools{
	Signal* makeWave(int length, int sampleRate, int frequency);
	Signal* readWav(std::string& fileName);
	Signal* normalize(short* data, int dataSize);
	Signal* combine(Signal& s1, Signal& s2);
}

