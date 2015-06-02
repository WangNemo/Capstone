// Capstone.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "definitions.h"

#include "testing.h"
#include "SignalBank.h"
#include "SignalGrid.h"
#include "GammatoneFilter.h"
#include "Correlogram.h"
#include "crossCorrelationSegmentation.h"
#include "IdealBinaryMask.h"
#include <ctime>
#include "LEGION.h"
#include "GroupingNetwork.h"
#include <vector>
#include "SoundTest.h"
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

struct AtExit
{
	~AtExit() { _CrtDumpMemoryLeaks(); }
} doAtExit;

int main(int argc, char* argv[], char* envp[]) {
	srand(static_cast <unsigned> (time(0)));

	SoundTest test("sounds", "results");
	test.run();

	_CrtDumpMemoryLeaks();
	return 0;

}

