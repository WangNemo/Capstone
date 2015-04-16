#include "stdafx.h"
#include "SignalBank.h"


SignalBank::SignalBank(int channels, int sampleRate, int samples) : CHANNELS(channels), SAMPLE_RATE(sampleRate), SAMPLES(samples)
{
	bank = new Signal[channels];
	forDeleting = new Signal*[channels];
}

Signal& SignalBank::operator[](int channel) {
	return *(forDeleting[channel]);
}

void SignalBank::add(Signal* sig, int index) {
	forDeleting[index] = sig;
	//bank[index] = *sig;
}


SignalBank::~SignalBank()
{
}
