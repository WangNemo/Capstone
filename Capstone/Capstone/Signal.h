#pragma once

class Signal
{
public:
	int SAMPLES, SAMPLE_RATE;
	double* signal;
	double& operator[](int sample);

	Signal() : SAMPLES(0), SAMPLE_RATE(0){}
	Signal(int samples, int sampleRate);
	void zeroOut();
	void reverse();
	~Signal();
};

