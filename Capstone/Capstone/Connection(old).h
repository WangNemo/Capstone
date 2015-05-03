#pragma once
#include "Oscillator(old).h"

class OscillatorOld;

class ConnectionOld {
private:
	double threshold;
public:
	OscillatorOld* one;
	OscillatorOld* two;
	double weight;

	ConnectionOld();
	ConnectionOld(double weight, OscillatorOld* one, OscillatorOld* two);

	double getWeightOne();

	double getWeightTwo();
};