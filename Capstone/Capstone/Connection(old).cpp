#include "stdafx.h"
#include "Connection(old).h"

ConnectionOld::ConnectionOld() { }
ConnectionOld::ConnectionOld(double weight, OscillatorOld* one, OscillatorOld* two) :
		weight(weight), one(one), two(two)  {}

double ConnectionOld::getWeightOne() {
		return (one->excitement > threshold) ?
			weight : 0;
	}

double ConnectionOld::getWeightTwo() {
		return (two->excitement > threshold) ?
			weight : 0;
	}
