#pragma once
#include "Definitions.h"
class Meddis
{
private:
	const double perm1 = 3.0;
	const double perm2 = 200;
	const double perm3 = 2000;
	const double REPLENISH_RATE = 5.05;
	const double LOSS_RATE = 2500;
	const double RETURN_RATE = 6580;
	const double PROCESS_RATE = 66.31;
	const double FIRING_PROBABILITY = 48000;
	const double TRANSMITTERS = 1;
public:
	Meddis() {};
	SignalBank filter(SignalBank bm, int sampleRate, int channels, int samples);
	double** unmodifiedFilter(SignalBank bm, int sampleRate, int channels, int samples);
	~Meddis();
};

