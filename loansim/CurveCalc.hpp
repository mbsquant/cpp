#ifndef CURVECALC_HPP
#define CURVECALC_HPP
#include <vector>
#include <string>

using namespace std;

	//get lifetime prepayment rate based on given length of CPR curve
double getlife(vector<double> vec);

	//convert the resulting big array into proper curves based on the set of curves types defined
	//return the vector of all curves required, each curve is a vector of double
vector<vector<double> > get_curve(vector<vector<vector<double> > > &data, string choice, unsigned int trail, unsigned int month, double orig_bal);

#endif
