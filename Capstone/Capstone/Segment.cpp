#include "stdafx.h"
#include "Segment.h"

Segment::Segment() {}
Segment::Segment(int segmentSize) : segmentSize(segmentSize)
{
	segment = new RowColumn*[segmentSize];
}

void Segment::add(RowColumn* pair) {
	if (inserted < segmentSize) {
		if (pair->col > maxFrame)
			maxFrame = pair->col;
		if (pair->col < minFrame)
			minFrame = pair->col;
		segment[inserted++] = pair;
	}
	else {
		print "Too many in seg" endl;
	}
	length = maxFrame - minFrame;
}

//int Segment::numActive(GroupingOscillator*** neuralGrid) {
//	active = 0;
//	for (int i = 0; i < segmentSize; i++) {
//		RowColumn* rc = segment[i];
//		if (neuralGrid[rc->row][rc->col]->excitement > SPIKE_THRESHOLD) {
//			active++;
//		}
//	}
//	return active;
//}

void Segment::deleteSeg() {
	for (int i = 0; i < segmentSize; i++) {
		delete segment[i];
	}
	delete[] segment;
}

void Segment::incrementAgree() {
	agree++;
}

void Segment::incrementDisagree(){
	disagree++;
}

void Segment::decide(double agreementRequirement) {
	if (segmentSize > 100)
	print /*"Agree: " << agree << "\tdisagree: " << disagree <<*/"Length: " << length << "\ttotal: " << segmentSize << "\tratio: " << (float)agree / segmentSize endl;
	//print agree << '\t' << disagree endl;
	fundamentalFreqencyMatch = ((double) agree) / segmentSize > agreementRequirement;
}

double Segment::agreementRatio() {
	return (double)agree / segmentSize;
}

//double Segment::power() {
//	double total = 0;
//
//}


bool Segment::overlapsWith(Segment& other){
	return (other.minFrame < minFrame && minFrame < other.maxFrame) ||
		   (other.minFrame < maxFrame && maxFrame < other.maxFrame) ||
		   (minFrame < other.minFrame && other.minFrame < maxFrame) ||
		   (minFrame < other.maxFrame && other.maxFrame < maxFrame) ||
		   (minFrame == other.minFrame && maxFrame == other.maxFrame);
}

Segment::~Segment()
{
	//TODO:
	// learn law of 3/5/0 or use c++11 pointers
	//delete[] segment; // breaks the vector because I don't know c++
}
