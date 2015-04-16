#pragma once
#include "FilterBank.h"
#include "Meddis.h"
#include "Definitions.h"
#include "Meddis.h"

class Cochleagram
{
private:
	FilterBank* bank;
	Meddis* meddis;
public:
	static const int CHANNELS = 128, MAX_FREQ = 5500, MIN_FREQ = 80;
	SignalBank* cochleagram;
	Cochleagram(Signal& signal, int samples, int sampleRate);
	~Cochleagram();
};

