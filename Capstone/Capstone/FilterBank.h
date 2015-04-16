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
	Signal logSpacedErbScale(double minErb, double maxErb);
public:
	const int CHANNELS, LOW, HIGH;
	Signal* channels;

	FilterBank(int channels, int lowFreq, int highFreq, int sampleRate);
	Signal* filter(Signal signal, int samples);
	~FilterBank();
};

