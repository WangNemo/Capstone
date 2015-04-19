#include "stdafx.h"
#include "HanningWindow.h"


HanningWindow::HanningWindow(int size) : SIZE(size)
{
	window = new Signal(size, 0);
	for (int i = 0; i < size; i++) {
		(*window)[i] = .5 * (cos(2 * PI * i / (size - 1)));
	}
}

void HanningWindow::filter(Signal& signal) {
	for (int sample = 0; sample < signal.SAMPLES; sample++) {
		double value = 0;
		for (int i = 0; i < SIZE; i++) {
			value += signal[sample] * (*window)[i];
		}
		signal[sample] = value;
	}
}



HanningWindow::~HanningWindow()
{
}
