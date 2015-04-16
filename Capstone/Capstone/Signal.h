#pragma once

class Signal
{
public:
	int samples;
	double* signal;
	double& operator[](int channel);

	Signal() {}
	Signal(int samples);
	~Signal();
};

