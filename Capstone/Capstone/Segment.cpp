#include "stdafx.h"
#include "Segment.h"

Segment::Segment() {}
Segment::Segment(int segmentSize) : segmentSize(segmentSize)
{
	segment = new RowColumn*[segmentSize];
}

int segmentSize;
RowColumn** segment;

void Segment::add(RowColumn* pair) {
	if (inserted < 0 || inserted >= segmentSize) {
		print "a";
	}
	segment[inserted++] = pair;
}

int Segment::numActive(GroupingOscillator*** neuralGrid) {
	active = 0;
	for (int i = 0; i < segmentSize; i++) {
		RowColumn* rc = segment[i];
		if (neuralGrid[rc->row][rc->col]->excitement > SPIKE_THRESHOLD) {
			active++;
		}
	}
	return active;
}

Segment::~Segment()
{
	//TODO:
	// learn law of 3/5/0 or use c++11 pointers
	//delete[] segment; // breaks the vector because I don't know c++
}
