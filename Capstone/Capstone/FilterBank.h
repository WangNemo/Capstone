#pragma once
#include "GammatoneFilter.h"
#include "Definitions.h"
#include "SignalBank.h"

class FilterBank
{
private:
	const float bandwidthCorrection = 1.019f;

	double erb(int frequency);
	double freqToErbScale(int frequency);
	int erbScaleToFreq(double erb);
	double* logSpacedErbScale(double minErb, double maxErb);
public:
	GammatoneFilter** bank;
	int CHANNELS, LOW, HIGH, SAMPLE_RATE;

	FilterBank(int channels, int lowFreq, int highFreq, int sampleRate);
	SignalBank* filter(Signal& signal);
	void filter(SignalBank& signal);
	Signal* reverse(SignalBank& inputBank);
	~FilterBank();
};

