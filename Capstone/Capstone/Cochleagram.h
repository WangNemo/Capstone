#pragma once
#include "FilterBank.h"
#include "Definitions.h"
#include "SignalBank.h"

class Cochleagram
{
private:
public:
	static const int CHANNELS = 128, MAX_FREQ = 5500, MIN_FREQ = 80;
	SignalBank* cochleagram;
	Cochleagram(Signal& signal, int sampleRate);
	~Cochleagram();
};

