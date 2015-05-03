#include "stdafx.h"
#include "Connection.h"

	Connection::Connection() { }
	Connection::Connection(double weight, Oscillator* one, Oscillator* two) :
		weight(weight), one(one), two(two)  {}

	double Connection::getWeightOne() {
		return (one->excitement > threshold) ?
			weight : 0;
	}

	double Connection::getWeightTwo() {
		return (two->excitement > threshold) ?
			weight : 0;
	}
