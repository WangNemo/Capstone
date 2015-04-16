#include "stdafx.h"
#include "Cochleagram.h"


Cochleagram::Cochleagram(Signal& signal, int samples, int sampleRate)
{
	bank = new FilterBank(CHANNELS, MIN_FREQ, MAX_FREQ, sampleRate);
	meddis = new Meddis();
	SignalBank* basilarMembrane = bank->filter(signal, samples);
	cochleagram = meddis->filter(*basilarMembrane, sampleRate, CHANNELS, samples);
}


Cochleagram::~Cochleagram()
{
}
