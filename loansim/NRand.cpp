#include "NRand.h"

double nrand(double mean, double sd) {
	static boost::lagged_fibonacci607 rng;
	boost::normal_distribution<> nor(mean, sd);
	boost::variate_generator<boost::lagged_fibonacci607&, boost::normal_distribution<> > nr(rng, nor);
	return nr();
}

double urand() {
	static boost::lagged_fibonacci607 rng;
	boost::uniform_01<boost::lagged_fibonacci607& > ur(rng);
	return ur();
}
