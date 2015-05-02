#include "stdafx.h"
#include "Oscillator.h"

Oscillator::Oscillator() {}

Oscillator::Oscillator(double excitement, double inhibition, double noise, double inputValue) :
excitement(excitement), inhibition(inhibition), randomNoise(noise), inputValue(inputValue)
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

void Oscillator::update() {
	double excitementChange = 3 * excitement - pow(excitement, 3) + 2 - inhibition + 
		inputValue + neighborWeights + randomNoise;
	double inhibitionChange = .04 * (9 * (1 + tanh(excitement / .1)) - inhibition);
	excitement += excitementChange * .1;
	inhibition += inhibitionChange * .1;
}

void Oscillator::updateNeighborWeights() {

}

Oscillator::~Oscillator()
{
}
