#include "stdafx.h"
#include "GlobalInhibitor.h"


GlobalInhibitor::GlobalInhibitor()
{
}

void GlobalInhibitor::update(bool isOscOverThreshold) {
	double inhibitionChange = ((int)isOscOverThreshold) - inhibition;
	inhibition += inhibitionChange * .1;
}


GlobalInhibitor::~GlobalInhibitor()
{
}
