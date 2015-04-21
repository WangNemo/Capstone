#include "stdafx.h"
#include "Cochleagram.h"
#include "Meddis.h"


Cochleagram::Cochleagram(Signal& signal, int sampleRate)
{
	FilterBank* bank = new FilterBank(CHANNELS, MIN_FREQ, MAX_FREQ, sampleRate);
	

	Meddis* meddis = new Meddis();
	SignalBank* basilarMembrane = bank->filter(signal);

	//for (int i = 0; i < CHANNELS; i++) {
	//	(*basilarMembrane)[i].normalize();
	//}

	cochleagram = basilarMembrane;
	//delete bank;
	//cochleagram = meddis->filter(*basilarMembrane);
	//delete meddis;
	//delete basilarMembrane;
}


Cochleagram::~Cochleagram()
{
	delete cochleagram;
}
