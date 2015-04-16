#pragma once
#include "GammatoneFilter.h"
#include "Definitions.h"

class FilterBank
{
private:
	const float bandwidthCorrection = 1.019f;
	GammatoneFilter* bank;

	double erb(int frequency);
	double freqToErbScale(int frequency);
	int erbScaleToFreq(double erb);
	double* logSpacedErbScale(double minErb, double maxErb);
public:
	const int CHANNELS, LOW, HIGH;
	SignalBank* channels;

	FilterBank(int channels, int lowFreq, int highFreq, int sampleRate);
	SignalBank* filter(Signal& signal, int samples);
	~FilterBank();
};

