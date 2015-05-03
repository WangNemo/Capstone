#pragma once
#include <math.h>
#include "Connection.h"
#include "Definitions.h"

class Oscillator
{
public:
	double prevInhi, prevExit;
	double excitement;
	double inhibition;
	double noise;
	double neighborWeights = 0;
	double inputValue;
	double potential = 1;
	int neighbors;

	Oscillator();
	Oscillator(double excitement, double inhibition, double noise, double inputValue, int neighbors);
	void update(double stepSize);
	void updateNeighborWeights(double* weights, double globalInhibition);
	void updatePotential(double weight, double stepSize);
	~Oscillator();
};

