#pragma once

#include "Definitions.h"

class Oscillator
{
private:
	bool leader = false;
public:
	double input;
	double excitement;
	Oscillator* up;
	Oscillator* down;
	Oscillator* left;
	Oscillator* right;
	double wUp = 0, wDown = 0, wLeft = 0, wRight = 0;
	bool hasSpiked = false;
	//bool largestSegment = false;
	int segment = -1;

	Oscillator(double randomExcitement, double input);

	bool isSpiking();
	bool isLeader();
	bool canSpike();
	bool isStimulated();

	int spike(int segment);
	void reset();
	void step();
	void makeLeader();
	void kill();

	double totalWeight();

	~Oscillator();
};

