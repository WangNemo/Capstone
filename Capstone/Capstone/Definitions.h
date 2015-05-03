#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include "Signal.h"

#define MAX(x,y)     ( ( x ) > ( y ) ? ( x ) : ( y ) )
#define MIN(x,y)     ( ( x ) < ( y ) ? ( x ) : ( y ) )

#define PI 3.1415926535897932384626433832795
#define print std::cout<<
#define end <<std::endl

#define SPIKING 1
#define SPIKE_THRESHOLD -1
#define MINIMUM_ACTIVITY -2
#define STEP .1


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

class intGrid {
public:
	int const ROWS, COLUMNS;
	int** grid;
	int operator()(int row, int col) {
		if (row >= ROWS || col >= COLUMNS)
			return 0;
		else return grid[row][col];
	}

	intGrid(int rows, int cols, int defaultValue) : ROWS(rows), COLUMNS(cols) {
		grid = new int*[ROWS];
		for (int i = 0; i < ROWS; i++) {
			grid[i] = new int[COLUMNS];
			std::fill_n(grid[i], COLUMNS, defaultValue);
		}
	}

	intGrid(int** grid, int rows, int cols) : grid(grid), ROWS(rows), COLUMNS(cols) {

	}

	void set(int row, int col, int value) {
		grid[row][col] = value;
	}

	void toFile(std::string path, int width = 1) {
		std::fstream os(path, std::ios::out);
		for (int i = ROWS - 1; i >= 0; --i)
		{
			for (int j = 0; j < COLUMNS; ++j)
			{
				int length = std::to_string((*this)(i, j)).length();
				os << (*this)(i, j);
				for (int x = 0; x < width - length; x++) {
					os << " ";
				}
			}
			os << "\n";
		}
	}

	~intGrid(){
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
	double standardDeviation(Signal& signal, double mean);
	double nonZeroGaussianRandom(double minMax);
}

