#include "stdafx.h"
#include "SignalBank.h"


SignalBank::SignalBank(int channels, int sampleRate, int samples) : CHANNELS(channels), SAMPLE_RATE(sampleRate), SAMPLES(samples)
{
	bank = new Signal*[channels];
}

Signal& SignalBank::operator[](int channel) {
	return *(bank[channel]);
}

void SignalBank::add(Signal* sig, int index) {
	/*if (bank[index] != NULL) {
		Signal* sig = bank[index];
		delete bank[index];
	}*/
	bank[index] = sig;
	//bank[index] = *sig;
}


SignalBank::~SignalBank()
{
	for (int i = 0; i < CHANNELS; i++) {
		delete bank[i];
	}
}
