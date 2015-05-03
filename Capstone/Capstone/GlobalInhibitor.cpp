#include "stdafx.h"
#include "GlobalInhibitor.h"


GlobalInhibitor::GlobalInhibitor()
{
}

void GlobalInhibitor::update(double stepSize, bool isOscOverThreshold) {
	double inhibitionChange = ((int)isOscOverThreshold) - inhibition;
	inhibition += inhibitionChange * stepSize;
}


GlobalInhibitor::~GlobalInhibitor()
{
}
