#include "stdafx.h"
#include "Oscillator.h"

Oscillator::Oscillator() {}

Oscillator::Oscillator(double excitement, double inhibition, double noise, double inputValue, int neighbors) :
excitement(excitement), inhibition(inhibition), noise(noise), inputValue(inputValue), neighbors(neighbors)
{
	//Signal sig(44100, 44100);
	//Signal sig2(44100, 44100);
	//for (int i = 0; i < 44100; i++) {
	//	update();
	//	sig[i] = this->excitement;
	//	sig2[i] = this->inhibition;
	//}
	//sig.save(std::string("oscil.wav"));
	//sig2.save(std::string("inhibition.wav"));
}

void Oscillator::update(double stepSize) {
	//double randomNoise = staticTools::nonZeroGaussianRandom(noise);
	
	prevInhi = inhibition;
	prevExit = excitement;

	double input = potential > .25 ? inputValue : 0;

	double excitementChange = 3 * excitement - pow(excitement, 3) + 2 - inhibition + 
		input + neighborWeights + noise;
	double inhibitionChange = .04 * (9 * (1 + tanh(excitement / .1)) - inhibition);

	excitement += excitementChange * stepSize;
	inhibition += inhibitionChange * stepSize;

}

void Oscillator::updateNeighborWeights(double* weights, double globalInhibition) {
	neighborWeights = 0;
	for (int i = 0; i < neighbors; i++) {
		neighborWeights += weights[i] - globalInhibition;
	}
	//if (neighborWeights < 0) neighborWeights = 0;
}

void Oscillator::updatePotential(double weight, double stepSize) {
	double change = (weight >= 0 ? (1 - potential) : 0) - .04 * potential;
	if (!(change <= DBL_MAX && change >= -DBL_MAX)) {
		int a = 3;
	}
	potential += change * stepSize;
}

Oscillator::~Oscillator()
{
}
