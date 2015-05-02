#pragma once
#include <math.h>
#include "Connection.h"


class Connection;


class Oscillator
{
public:
	double excitement;
	double inhibition;
	double randomNoise;
	double neighborWeights = 0;
	double inputValue;
	Connection* neighbors;
	int numNeighbors;

	Oscillator();
	Oscillator(double excitement, double inhibition, double noise, double inputValue);
	void update();
	void updateNeighborWeights();
	~Oscillator();
};

