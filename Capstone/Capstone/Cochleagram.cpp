#include "stdafx.h"
#include "Cochleagram.h"
#include "Meddis.h"


Cochleagram::Cochleagram(Signal& signal, int sampleRate)
{
	FilterBank* bank = new FilterBank(CHANNELS, MIN_FREQ, MAX_FREQ, sampleRate);
	Meddis* meddis = new Meddis();
	SignalBank* basilarMembrane = bank->filter(signal);
	delete bank;
	cochleagram = meddis->filter(*basilarMembrane);
	delete meddis;
	delete basilarMembrane;
}


Cochleagram::~Cochleagram()
{
	delete cochleagram;
}
