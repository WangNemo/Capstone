#include "stdafx.h"
#include "Oscillator.h"

Oscillator::Oscillator() {}

Oscillator::Oscillator(double excitement, double inhibition, double noise, double inputValue, int neighbors) :
excitement(excitement), inhibition(inhibition), noise(noise), inputValue(inputValue), neighbors(neighbors)
{
	Signal sig(44100, 44100);
	Signal sig2(44100, 44100);
	Signal sig3(44100, 44100);
	for (int i = 0; i < 44100; i++) {
		update(.1);
		sig[i] = this->excitement;
		sig2[i] = this->inhibition;
		sig3[i] = this->potential;
		updatePotential(prevExit - 1, .1);
	}
	sig.save(std::string("oscil.wav"));
	sig2.save(std::string("inhibition.wav"));
	sig3.save(std::string("potential.wav"));
}

Oscillator::Oscillator(double excitement, double inhibition, double noise, double inputValue, int neighbors, int samplesToSave) :
excitement(excitement), inhibition(inhibition), noise(noise), inputValue(inputValue), neighbors(neighbors), samplesToSave(samplesToSave)
{
	exitWav = new Signal(samplesToSave, 44100);
	inhibitWav = new Signal(samplesToSave, 44100);
	potWav = new Signal(samplesToSave, 44100);
	//Signal sig2(44100, 44100);
	//Signal sig3(44100, 44100);
	//for (int i = 0; i < 44100; i++) {
	//	update(.1);
	//	sig[i] = this->excitement;
	//	sig2[i] = this->inhibition;
	//	sig3[i] = this->potential;
	//	updatePotential(prevExit - 1, .1);
	//}
	//sig.save(std::string("oscil.wav"));
	//sig2.save(std::string("inhibition.wav"));
	//sig3.save(std::string("potential.wav"));
}

void Oscillator::update(double stepSize) {
	double randomNoise = staticTools::nonZeroGaussianRandom(.02);
	
	prevInhi = inhibition;
	prevExit = excitement;

	double input = potential > .25 || inputValue < 0 ? inputValue : 0;

	double adjustment = input + neighborWeights + randomNoise;// input > 0 ? randomNoise : 0;

	if (adjustment <= 0) {
		int a = 5;
	}

	double excitementChange = 3 * excitement - pow(excitement, 3) + 2 - inhibition + 
		adjustment;
	double inhibitionChange = .04 * (9.0 * (1 + tanh(excitement / .1)) - inhibition);

	excitement += excitementChange * stepSize;

	inhibition += inhibitionChange * stepSize;
	if (samplesSaved < samplesToSave) {
		(*exitWav)[samplesSaved] = this->excitement;
		(*inhibitWav)[samplesSaved] = this->inhibition;
		(*potWav)[samplesSaved++] = this->potential;
	}
}

void Oscillator::updateNeighborWeights(double* weights, double globalInhibition) {
	neighborWeights = 0;
	for (int i = 0; i < neighbors; i++) {
		neighborWeights += weights[i] - globalInhibition;
	}
}

void Oscillator::updatePotential(double weight, double stepSize) {
	double change = (weight >= 0 ? (1 - potential) : 0) - .02 * potential;
	if (!(change <= DBL_MAX && change >= -DBL_MAX)) {
		int a = 3;
	}
	potential += change * stepSize;
}

void Oscillator::saveWavs(std::string name) {
	exitWav->save(name + ".exit.wav");
	inhibitWav->save(name + ".inhibit.wav");
	potWav->save(name + ".pot.wav");
}

Oscillator::~Oscillator()
{
}
