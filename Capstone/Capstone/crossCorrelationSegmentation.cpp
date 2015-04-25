#include "stdafx.h"
#include "crossCorrelationSegmentation.h"


crossCorrelationSegmentation::crossCorrelationSegmentation(SignalGrid& correlogram)
	: correlogram(correlogram)
{
	segmentGrid = new intGrid(correlogram.FRAMES, correlogram.CHANNELS, UNASSIGNED);
	for (int row = 0; row < correlogram.FRAMES; row++) {
		for (int col = 0; col < correlogram.CHANNELS; col++) {
			traverse(row, col);
		}
		//print row end;
	}
}

void crossCorrelationSegmentation::traverse(int row, int col) {
	Signal& thisCorrelation = correlogram[row][col];
	if (thisCorrelation[0] == 0){
		segmentGrid->set(row, col, 0);
	}
	int group = (*segmentGrid)(row, col);
	bool matchFound = false;
	int nextRow = row - 1;
	int nextCol = col + 0;
	double threshold = .5;

	if (nextRow >= 0) {
		if ((*segmentGrid)(nextRow, nextCol) == UNASSIGNED) {
			Signal& nextCorrelation = correlogram[nextRow][nextCol];
			double relation = crossCorrelation(thisCorrelation, nextCorrelation);
			if (relation > threshold) {
				if (!matchFound) group = group > UNASSIGNED ? group : ++groups;
				(*segmentGrid).set(row, col, group);
				(*segmentGrid).set(nextRow, nextCol, group);
				traverse(nextRow, nextCol);
				matchFound = true;
			}
		}
	}

	nextRow = row + 1;
	if (nextRow < correlogram.FRAMES) {
		if ((*segmentGrid)(nextRow, nextCol) == UNASSIGNED) {
			Signal& nextCorrelation = correlogram[nextRow][nextCol];
			double relation = crossCorrelation(thisCorrelation, nextCorrelation);
			if (relation > threshold) {
				if (!matchFound) group = group > UNASSIGNED ? group : ++groups;
				(*segmentGrid).set(row, col, group);
				(*segmentGrid).set(nextRow, nextCol, group);
				traverse(nextRow, nextCol);
				matchFound = true;
			}
		}
	}

	nextRow = row;
	nextCol = col + 1;
	if (nextCol >= 0) {
		if ((*segmentGrid)(nextRow, nextCol) == UNASSIGNED) {
			Signal& nextCorrelation = correlogram[nextRow][nextCol];
			double relation = crossCorrelation(thisCorrelation, nextCorrelation);
			if (relation > threshold) {
				if (!matchFound) group = group > UNASSIGNED ? group : ++groups;
				(*segmentGrid).set(row, col, group);
				(*segmentGrid).set(nextRow, nextCol, group);
				traverse(nextRow, nextCol);
				matchFound = true;
			}
		}
	}

	nextCol = col - 1;
	if (nextCol < correlogram.CHANNELS) {
		if ((*segmentGrid)(nextRow, nextCol) == UNASSIGNED) {
			Signal& nextCorrelation = correlogram[nextRow][nextCol];
			double relation = crossCorrelation(thisCorrelation, nextCorrelation);
			if (relation > threshold) {
				if (!matchFound) group = group > UNASSIGNED ? group : ++groups;
				(*segmentGrid).set(row, col, group);
				(*segmentGrid).set(nextRow, nextCol, group);
				traverse(nextRow, nextCol);
				matchFound = true;
			}
		}
	}

	// check up
	// check down
	// check left
	// check right
}

double crossCorrelationSegmentation::crossCorrelation(Signal& one, Signal& two) {
	if (one[0] == 0 || two[0] == 0)
		return 0;

	double sum = 0;
	for (int i = 0; i < one.SAMPLES - 1; i++) {
		sum += one[i] * two[i + 1];
	}
	return sum / one.SAMPLES;
}


boolGrid* crossCorrelationSegmentation::getBinaryMask(int group) {
	bool** binaryMask = new bool*[segmentGrid->ROWS];
	for (int row = 0; row < segmentGrid->ROWS; row++) {
		binaryMask[row] = new bool[segmentGrid->COLUMNS];
		for (int col = 0; col < segmentGrid->COLUMNS; col++) {
			binaryMask[row][col] = (*segmentGrid)(row, col) == group;
			//if(binaryMask[row][col]) print group end;
		}
	}
	boolGrid* idealBinaryMask1 = new boolGrid(binaryMask, segmentGrid->ROWS, segmentGrid->COLUMNS);
	return idealBinaryMask1;
}




crossCorrelationSegmentation::~crossCorrelationSegmentation()
{
}
