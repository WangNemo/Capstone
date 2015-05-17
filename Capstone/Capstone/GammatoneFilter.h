#pragma once

#include "Definitions.h"

/*
	Implementation based on work done by:
	Ning Ma, University of Sheffield
	http://staffwww.dcs.shef.ac.uk/people/N.Ma/resources/gammatone/gammatone_c.c
*/

class GammatoneFilter
{
private:
	static const int ORDER = 4;

	double orders[ORDER]; // 4th order gammatone
	Complex priorResults[ORDER] = {};

	double frequencyCenter;

	double exp2;
	double radianFrequency;
	double radianTimeStep;
	double radianSize;
	double gain;

	double cosFreq;
	double sinFreq;

	void shiftPriorResults(Complex& newResult);
	void filterComplex(Complex& shiftedSample);
public:
	GammatoneFilter() {};
	GammatoneFilter(int frequencyCenter, double bandwidth, int sampleRate);
	Signal* filter(Signal& signal, bool envelope);
	void filterInplace(Signal& signal);
	~GammatoneFilter();
};

