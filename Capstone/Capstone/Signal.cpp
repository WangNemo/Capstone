#include "stdafx.h"
#include "Signal.h"


double& Signal::operator[](int channel) {
	return signal[channel];
}

Signal::Signal(int samples) : samples(samples) {
	signal = new double[samples];
}
Signal::~Signal(){

}
