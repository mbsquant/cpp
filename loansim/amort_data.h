#ifndef AMORT_DATA_H
#define AMORT_DATA_H

//# include "LoanDef.h"
using namespace std;

//Amortization data structure with default constructor and change minimum payment member function
class amort_data {
public:
	double teaser_rate;		//teaser rate
	double teaser_term;		//teaser term
	double init_coupon;		//initial coupon
	double margin;			//margin
	double reset_freq;		//reset frequency for interest rate
	double init_cap;		//initial cap for interest rate
	double perd_cap;		//periodic cap for interest rate
	double life_cap;		//life time cap for interest rate
	double life_flr;		//life time floor for interest rate
	unsigned int amort_term;			//amortization term
	unsigned int io;					//IO term
	double neg_limit;		//negative amortization limit
	double mini_pay;		//minimal payment for NEG (Negam) loan
	
	//default constructor
	amort_data() {
		teaser_rate = 0.;	
		teaser_term = 0.;
		init_coupon = 0.;
		margin = 0.;
		reset_freq = 0.;
		init_cap = 0.001;
		perd_cap = 0.;
		life_cap = 0.;
		life_flr = 0.;
		amort_term = 0;
		io = 0;
		neg_limit=1.;
		mini_pay=0.;
	}

	//another constructor
	amort_data(double tr, double tt, double ic, double ma, double rf, double inc, double pec, double lic, double lif, unsigned int at, unsigned int iox, double n_l, double rct) {
		teaser_rate = tr;
		teaser_term = tt;
		init_coupon = ic;
		margin = ma;
		reset_freq = rf;
		init_cap = inc;
		perd_cap = pec;
		life_cap = lic;
		life_flr = lif;
		amort_term = at;
		io = iox;
		neg_limit=n_l;
		mini_pay = rct;
	}

	//reset minimum payment
	void change_mini_pay(double mp) {
		mini_pay = mp;
	}

	//change negative amortization limit
	void change_neg_limit(double nl){
		neg_limit = nl;
	}

};

#endif
