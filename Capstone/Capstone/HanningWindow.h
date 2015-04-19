#pragma once
#include "Definitions.h"

class HanningWindow
{
private:
	int SIZE;
public:
	Signal* window;
	HanningWindow(int size);
	void filter(Signal& signal);
	~HanningWindow();
};

