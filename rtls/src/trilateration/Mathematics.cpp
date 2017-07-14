
#include <math.h>
#include "Mathematics.h"

static const double EPSINON = 1e-6;

bool fequal(double x, double y) {
	if (fabs(x - y) >= EPSINON)
		return false;
	return true;
}
