#pragma once

class Signal
{
public:
	int SAMPLES;
	double* signal;
	double& operator[](int channel);

	Signal() : SAMPLES(0){}
	Signal(int samples);
	~Signal();
};

