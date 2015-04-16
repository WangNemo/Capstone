#include "stdafx.h"
#include "Signal.h"


double& Signal::operator[](int channel) {
	return signal[channel];
}

Signal::Signal(int samples) : SAMPLES(samples) {
	signal = new double[samples];
}
Signal::~Signal(){

}
