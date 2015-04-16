#include "stdafx.h"
#include "Cochleagram.h"


Cochleagram::Cochleagram(Signal& signal, int sampleRate)
{
	bank = new FilterBank(CHANNELS, MIN_FREQ, MAX_FREQ, sampleRate);
	meddis = new Meddis();
	SignalBank* basilarMembrane = bank->filter(signal);
	cochleagram = meddis->filter(*basilarMembrane);
}


Cochleagram::~Cochleagram()
{
}
