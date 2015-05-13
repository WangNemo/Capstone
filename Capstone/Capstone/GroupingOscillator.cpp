#include "stdafx.h"
#include "GroupingOscillator.h"


GroupingOscillator::GroupingOscillator() : segment(*new Segment()) {
	inputValue = -1000;
}

GroupingOscillator::GroupingOscillator(double inputValue, Segment& segment)
	:inputValue(inputValue), segment(segment)
{

}

void GroupingOscillator::update() {
	if (inputValue >= 0) {
		double randomNoise = staticTools::nonZeroGaussianRandom(.02);


		double segmentWeight = 0;
		if (segment.active > 0) {
			segmentWeight = (((float)segment.active) / segment.segmentSize > .5) ?
				MAJORITY_SPIKE_WEIGHT : SINGLE_SPIKE_WEIGHT;
		}

		double adjustment = inputValue + neighborWeights + randomNoise;// input > 0 ? randomNoise : 0;

		double excitementChange = 3 * excitement - pow(excitement, 3) + 2 - inhibition +
			adjustment;
		double inhibitionChange = .04 * (9.0 * (1 + tanh(excitement / .1)) - inhibition);

		excitement += excitementChange * STEP;

		inhibition += inhibitionChange * STEP;
	}
	//if (samplesSaved < samplesToSave) {
	//	(*exitWav)[samplesSaved] = this->excitement;
	//	(*inhibitWav)[samplesSaved] = this->inhibition;
	//	(*potWav)[samplesSaved++] = this->potential;
	//}
}


GroupingOscillator::~GroupingOscillator()
{

}
