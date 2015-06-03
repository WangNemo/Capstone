#include "stdafx.h"
#include "crossCorrelationSegmentation.h"


//crossCorrelationSegmentation::crossCorrelationSegmentation(SignalGrid& correlogram)
//	: correlogram(correlogram)
//{
//	print "cross correlation" endl;
//	segmentGrid = new intGrid(correlogram.CHANNELS, correlogram.FRAMES, UNASSIGNED);
//	for (int row = 0; row < correlogram.CHANNELS; row++) {
//		for (int col = 0; col < correlogram.FRAMES; col++) {
//			traverse(row, col);
//		}
//		print row endl;
//	}
//}
crossCorrelationSegmentation::crossCorrelationSegmentation(boolGrid* grid) : booleanGrid(grid) {
	segmentGrid = new intGrid(booleanGrid->ROWS, booleanGrid->COLUMNS, UNASSIGNED);
	for (int row = 0; row < booleanGrid->ROWS; row++) {
		for (int col = 0; col < booleanGrid->COLUMNS; col++) {
			traverse(row, col);
		}
	}
}

void crossCorrelationSegmentation::traverse(int channel, int frame) {
	if (!(*booleanGrid)(channel, frame))
		return;

	int group = (*segmentGrid)(channel, frame);
	bool matchFound = false;
	int nextFrame = frame - 1;
	int nextChannel = channel + 0;

	if (nextFrame >= 0) {
		if ((*segmentGrid)(nextChannel, nextFrame) == UNASSIGNED) {
			if ((*booleanGrid)(nextChannel, nextFrame)) {
				if (!matchFound) group = group > UNASSIGNED ? group : ++groups;
				(*segmentGrid).set(channel, frame, group);
				(*segmentGrid).set(nextChannel, nextFrame, group);
				traverse(nextChannel, nextFrame);
				matchFound = true;
			}
		}
	}

	nextFrame = frame + 1;
	if (nextFrame < booleanGrid->COLUMNS) {
		if ((*segmentGrid)(nextChannel, nextFrame) == UNASSIGNED) {
			if ((*booleanGrid)(nextChannel, nextFrame)) {
				if (!matchFound) group = group > UNASSIGNED ? group : ++groups;
				(*segmentGrid).set(channel, frame, group);
				(*segmentGrid).set(nextChannel, nextFrame, group);
				traverse(nextChannel, nextFrame);
				matchFound = true;
			}
		}
	}

	nextFrame = frame;
	nextChannel = channel + 1;
	if (nextChannel < booleanGrid->ROWS) {
		if ((*segmentGrid)(nextChannel, nextFrame) == UNASSIGNED) {
			if ((*booleanGrid)(nextChannel, nextFrame)) {
				if (!matchFound) group = group > UNASSIGNED ? group : ++groups;
				(*segmentGrid).set(channel, frame, group);
				(*segmentGrid).set(nextChannel, nextFrame, group);
				traverse(nextChannel, nextFrame);
				matchFound = true;
			}
		}
	}

	nextChannel = channel - 1;
	if (nextChannel > 0) {
		if ((*segmentGrid)(nextChannel, nextFrame) == UNASSIGNED) {
			if ((*booleanGrid)(nextChannel, nextFrame)) {
				if (!matchFound) group = group > UNASSIGNED ? group : ++groups;
				(*segmentGrid).set(channel, frame, group);
				(*segmentGrid).set(nextChannel, nextFrame, group);
				traverse(nextChannel, nextFrame);
				matchFound = true;
			}
		}
	}

	// check up
	// check down
	// check left
	// check right
}

int crossCorrelationSegmentation::getLargestSegment() {
	int numInLargest = 0;
	int largest = 0;

	for (int i = 1; i <= groups; i++) {
		int count = 0;
		for (int row = 0; row < booleanGrid->ROWS; row++) {
			for (int col = 0; col < booleanGrid->COLUMNS; col++) {
				if ((*segmentGrid)(row, col) == i) {
					count++;
				}
			}
		}
		if (count > numInLargest) {
			numInLargest = count;
			largest = i;
		}
	}
	return largest;
}

int crossCorrelationSegmentation::segmentSize(int group) {
	int count = 0;
	for (int row = 0; row < booleanGrid->ROWS; row++) {
		for (int col = 0; col < booleanGrid->COLUMNS; col++) {
			if ((*segmentGrid)(row, col) == group) {
				count++;
			}
		}
	}
	return count;
}


boolGrid* crossCorrelationSegmentation::getBinaryMask(int group) {
	bool** binaryMask = new bool*[segmentGrid->ROWS];
	for (int row = 0; row < segmentGrid->ROWS; row++) {
		binaryMask[row] = new bool[segmentGrid->COLUMNS];
		for (int col = 0; col < segmentGrid->COLUMNS; col++) {
			binaryMask[row][col] = (*segmentGrid)(row, col) == group;
			//if(binaryMask[row][col]) print group endl;
		}
	}
	//print segmentGrid->ROWS << '\t' << segmentGrid->COLUMNS endl;
	boolGrid* idealBinaryMask1 = new boolGrid(binaryMask, segmentGrid->ROWS, segmentGrid->COLUMNS);
	return idealBinaryMask1;
}

int crossCorrelationSegmentation::timeLength(int group){
	int min = segmentGrid->COLUMNS;
	int max = 0;
	for (int i = 0; i < segmentGrid->COLUMNS && min == segmentGrid->COLUMNS; i++) {
		bool found = false;
		for (int channel = 0; channel < segmentGrid->ROWS && !found; channel++) {
			found = (*segmentGrid)(channel, i) == group;
		}
		if (found) {
			min = i;
		}
	}
	for (int i = segmentGrid->COLUMNS; i > 0 && max == 0; i--) {
		bool found = false;
		for (int channel = 0; channel < segmentGrid->ROWS && !found; channel++) {
			found = (*segmentGrid)(channel, i) == group;
		}
		if (found) {
			max = i;
		}
	}
	return max - min;
}


void crossCorrelationSegmentation::writeSegmentText(std::string name)
{
	segmentGrid->toFile(name);
}

crossCorrelationSegmentation::~crossCorrelationSegmentation()
{
	delete segmentGrid;
	delete booleanGrid;
}
