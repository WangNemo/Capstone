#include "stdafx.h"
#include "SignalBank.h"


SignalBank::SignalBank(int channels)
{
	bank = new Signal[channels];
	forDeleting = new Signal*[channels];
}

Signal& SignalBank::operator[](int channel) {
	return bank[channel];
}

void SignalBank::add(Signal* sig, int index) {
	forDeleting[index] = sig;
	bank[index] = *sig;
}


SignalBank::~SignalBank()
{
}
