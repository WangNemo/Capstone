#include "stdafx.h"
#include "FilterBank.h"


FilterBank::FilterBank(int channels, int lowFreq, int highFreq, int sampleRate) : 
	CHANNELS(channels), LOW(lowFreq), HIGH(highFreq) {
	bank = new GammatoneFilter[CHANNELS];

	double lowErb = freqToErbScale(LOW);
	double highErb = freqToErbScale(HIGH);
	//print lowErb << " to " << highErb end;

	double* erbScale = logSpacedErbScale(lowErb, highErb);
	for (int i = 0; i < CHANNELS; i++) {
		int frequency = round(erbScaleToFreq(erbScale[i]));
		double bandwith = erb(frequency) * bandwidthCorrection;
		//print i << "\t" << frequency << "\t" << erbScale[i] end;
		bank[i] = *(new GammatoneFilter(frequency, bandwith, sampleRate));
	}

	delete[] erbScale;
}

Signal* FilterBank::filter(Signal signal, int samples){
	channels = new Signal[CHANNELS];
	for (int i = 0; i < CHANNELS; i++) {
		channels[i] = bank->filter(signal, samples);
	}
	return channels;
}

double FilterBank::erb(int frequency){
	return 24.7 * ( 4.37e-3 * frequency + 1);
}

double FilterBank::freqToErbScale(int frequency){
	double f = 4.37e-3 * frequency + 1;
	return log10(f) * 21.4;
}

int FilterBank::erbScaleToFreq(double erb){
	double e = erb / 21.4;
	return (pow(10, e) - 1) / 4.37e-3;
}

double* FilterBank::logSpacedErbScale(double minErb, double maxErb){
	double erbRange = maxErb - minErb;
	double erbStep = erbRange / (CHANNELS - 1);
	double* spacedErb = new double[CHANNELS];

	spacedErb[0] = minErb;
	for (int i = 1; i < CHANNELS; i++) {
		spacedErb[i] = spacedErb[i - 1] + erbStep;
	}
	return spacedErb;
}

FilterBank::~FilterBank()
{
}
