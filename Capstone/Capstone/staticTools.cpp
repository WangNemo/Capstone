#include "stdafx.h"

#include "Definitions.h"

namespace staticTools{
	Signal makeWave(int length, int frequency) {
		Signal wave = new double[length];

		for (int i = 0; i < length; i++) {
			wave[i] = sin(PI * 2 * frequency * i / length);
		}
		return wave;
	}
}