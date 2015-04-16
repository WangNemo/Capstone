#pragma once
#include "Definitions.h"

class SignalBank
{
private: 
	Signal** forDeleting;
	Signal* bank;
public:
	SignalBank(){};
	SignalBank(int channels);
	Signal& operator[](int channel);
	void add(Signal* sig, int index);
	~SignalBank();
};

