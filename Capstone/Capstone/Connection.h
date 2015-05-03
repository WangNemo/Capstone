#pragma once

#include "Oscillator.h"

class Connection
{
private:
	double threshold;
public:
	Oscillator* one;
	Oscillator* two;
	double weight;

	Connection();
	Connection(double weight, Oscillator* one, Oscillator* two);

	double getWeightOne();

	double getWeightTwo();
};

