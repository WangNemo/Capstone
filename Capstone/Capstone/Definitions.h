#pragma once


#ifdef _DEBUG
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif  // _DEBUG

#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include "Signal.h"
#include <vector>
#include <windows.h>

#define MAX(x,y)     ( ( x ) > ( y ) ? ( x ) : ( y ) )
#define MIN(x,y)     ( ( x ) < ( y ) ? ( x ) : ( y ) )

#define PI 3.1415926535897932384626433832795
#define print std::cout<<
#define endl <<std::endl

#define SPIKING 1
#define SPIKE_THRESHOLD -1
#define MINIMUM_ACTIVITY -2
#define STEP .1
#define MAJORITY_SPIKE_WEIGHT 4
#define SINGLE_SPIKE_WEIGHT .1
#define MUTUAL_EXCITEMENT .5
#define MUTUAL_INHIBITION -.5


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

struct RowColumn {
	int row;
	int col;

	RowColumn() {}
	RowColumn(int row, int col)
		:row(row), col(col) {}
};

struct SeparationResult {
	double foregroundEnergy = 1;
	double backgroundEnergy = 1;
	double foregroundNoise = 1;
	double backgroundNoise = 1;
	double mean = 1;

	std::string toString() {
		return std::to_string(foregroundEnergy) + "\t" + std::to_string(foregroundNoise) + "\t" + std::to_string(backgroundEnergy) + "\t" + std::to_string(backgroundNoise) + "\t" + std::to_string(mean);
	}
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
			delete[] grid[i];
		}
		delete[] grid;
	}

	void toFile(std::string path) {
		std::fstream os(path, std::ios::out);
		for (int i = ROWS - 1; i >= 0; --i)
		{
			for (int j = 0; j < COLUMNS; ++j)
			{
				os << (*this)(i, j);
				os << "\t";
			}
			os << "\n";
		}
	}

	int countOver(double threshold) {
		int count = 0;
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLUMNS; j++) {
				if(grid[i][j] > threshold)
					count++;
			}
		}
		return count;
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

	void toFile(std::string path) {
		std::fstream os(path, std::ios::out);
		for (int i = ROWS - 1; i >= 0; --i)
		{
			for (int j = 0; j < COLUMNS; ++j)
			{
				os << (*this)(i, j);
				os << "\t";
			}
			os << "\n";
		}
	}

	~intGrid(){
		for (int i = 0; i < ROWS; i++) {
			delete[] grid[i];
		}
		delete[] grid;
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

	boolGrid* intersect(boolGrid& other) {
		boolGrid* result = nullptr;

		if (ROWS == other.ROWS && COLUMNS == other.COLUMNS) {
			bool** newGrid = new bool*[ROWS];
			for (int row = 0; row < ROWS; row++) {
				newGrid[row] = new bool[COLUMNS];
				for (int col = 0; col < COLUMNS; col++) {
					newGrid[row][col] = (grid[row][col] && other(row, col));
				}
			}
			result = new boolGrid(newGrid, ROWS, COLUMNS);
		}
		return result;
	}

	void invert() {
		for (int row = 0; row < ROWS; row++) {
			for (int col = 0; col < COLUMNS; col++) {
				grid[row][col] = !grid[row][col];
			}
		}
	}

	~boolGrid(){
		for (int i = 0; i < ROWS; i++) {
			delete[] grid[i];
		}
		delete[] grid;
	}

	void toFile(std::string path) {
		std::fstream os(path, std::ios::out);
		for (int i = ROWS - 1; i >= 0; --i)
		{
			for (int j = 0; j < COLUMNS; ++j)
			{
				os << (*this)(i, j);
				os << "\t";
			}
			os << "\n";
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
	std::vector<std::string> get_all_files_names_within_folder(std::string folder);
	std::wstring s2ws(const std::string& str);
	std::string ws2s(const std::wstring& wstr);
}

