#pragma once

#include "Meddis.h"
#include "Definitions.h"
#include "GammatoneFilter.h"
#include "Meddis.h"
#include "Cochleagram.h"

#define erb(x)         ( 24.7 * ( 4.37e-3 * ( x ) + 1.0 ) )
#define BW_CORRECTION      1.0190

namespace test{

	void testMeddis();
	void testGammatoneFilter();
}
