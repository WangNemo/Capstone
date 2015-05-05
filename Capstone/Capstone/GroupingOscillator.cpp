#include "stdafx.h"
#include "GroupingOscillator.h"

GroupingOscillator::GroupingOscillator()
{

}

void GroupingOscillator::update(double stepSize) {
	double randomNoise = staticTools::nonZeroGaussianRandom(.02);

	double input = potential > .1 ? inputValue : 0;

	double adjustment = input + neighborWeights + randomNoise;// input > 0 ? randomNoise : 0;

	if (adjustment <= 0) {
		int a = 5;
	}

	double excitementChange = 3 * excitement - pow(excitement, 3) + 2 - inhibition +
		adjustment;
	double inhibitionChange = .04 * (9.0 * (1 + tanh(excitement / .1)) - inhibition);

	excitement += excitementChange * stepSize;

	inhibition += inhibitionChange * stepSize;
	//if (samplesSaved < samplesToSave) {
	//	(*exitWav)[samplesSaved] = this->excitement;
	//	(*inhibitWav)[samplesSaved] = this->inhibition;
	//	(*potWav)[samplesSaved++] = this->potential;
	//}
}


GroupingOscillator::~GroupingOscillator()
{
}
