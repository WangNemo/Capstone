#pragma once
#include "Definitions.h"
#include "SignalBank.h"
#include "Signal.h"

class SignalBank
{
private: 
	Signal** forDeleting;
	Signal* bank;
public:
	const int CHANNELS, SAMPLE_RATE, SAMPLES;
	SignalBank() : CHANNELS(0), SAMPLE_RATE(0), SAMPLES(0){};
	SignalBank(int channels, int sampleRate, int samples);
	Signal& operator[](int channel);
	void add(Signal* sig, int index);
	~SignalBank();
};

