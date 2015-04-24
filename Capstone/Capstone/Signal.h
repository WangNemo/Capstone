#pragma once

class Signal
{
private: 
	double getCorrelation(int startingSample, Signal& window, int lag);
public:
	int SAMPLES, SAMPLE_RATE;
	double* signal;
	double& operator[](int sample);

	//Signal() : SAMPLES(0), SAMPLE_RATE(0){}
	Signal(int samples, int sampleRate);
	void zeroOut();
	void reverse();
	void normalize();
	void scale(double scalar);
	void trim(int samples);
	void add(Signal& other);
	Signal* minus(Signal& other);
	Signal* autoCorrelate(int lagMS, int startingMS, Signal& window, double threshold);
	~Signal();
};

