#include "stdafx.h"
#include "Oscillator.h"


Oscillator::Oscillator(double randomExcitement, double input)
	: excitement(randomExcitement), input(input)
{
}

bool Oscillator::isSpiking() {
	return excitement == SPIKING;
}

bool Oscillator::isLeader() {
	return leader;
}

bool Oscillator::canSpike() {
	return false;
}

bool Oscillator::isStimulated() {
	return input > 0;
}

int Oscillator::spike(int segment) {
	int numSpike = 0;
	if (!isSpiking() && input > 0) {
		hasSpiked = true;
		this->segment = segment;
		numSpike++;
		excitement = SPIKING;
		if (wUp > 0 && up != nullptr && !up->isSpiking()) {
			int addSpike = up->spike(segment);
			numSpike += addSpike;
		}
		if (wDown > 0 && down != nullptr && !down->isSpiking()) {
			int addSpike = down->spike(segment);
			numSpike += addSpike;
		}
		if (wLeft > 0 && left != nullptr && !left->isSpiking()) {
			int addSpike = left->spike(segment);
			numSpike += addSpike;
		}
		if (wRight > 0 && right != nullptr && !right->isSpiking()) {
			int addSpike = right->spike(segment);
			numSpike += addSpike;
		}
	}
	return numSpike;
}

void Oscillator::reset() {
	excitement = MINIMUM_ACTIVITY;
}

void Oscillator::step() {
	if (isStimulated())
		excitement += (SPIKE_THRESHOLD - excitement) * STEP;
	else
		excitement = -5;
}

void Oscillator::makeLeader() {
	leader = true;
}

void Oscillator::kill() {
	leader = false;
	input = -5;
	excitement = -5;
	segment = -1;
	if (wUp > 0 && up != nullptr && up->isSpiking()) {
		up->kill();
	}
	if (wDown > 0 && down != nullptr && down->isSpiking()) {
		down->kill();
	}
	if (wLeft > 0 && left != nullptr && left->isSpiking()) {
		left->kill();
	}
	if (wRight > 0 && right != nullptr && right->isSpiking()) {
		right->kill();
	}
}

double Oscillator::totalWeight() {
	return wUp + wDown + wLeft + wRight;
}

Oscillator::~Oscillator()
{
}
