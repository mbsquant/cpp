#ifndef NEWSIM_H
#define NEWSIM_H

#include "NextStepCalc.h"
#include "Assumption.hpp"

using namespace std;

//class of cashflow types available and core simulation member function
//typedef vector<vector<vector<double > > >::iterator vp;

enum CFType{
    INT_PAY = 0, //interest payment
	PRIN_PAY,	 //principal payment
	RE_BAL,		 //remaining balance
	PRE_PAY,	 //prepay
	DEF_PAY,	 //default
	DELINQ_M,	 //delinquent curve
	DEF_AMT,     //default amount
	END_CF_TYPE  //end of cashflow types
};
	//simulation core function
void Deal_Sim(vector<LoanDef> &vl, 
            // how to change hpahist
        Assumption &assump,
        unordered_map<string, vector<double> > &sev_coeff, unsigned int numOfMonths, NextStepCalcML &nsc, vector<vector<vector<double> > > &res, unsigned int run_bg, unsigned int run_ed);

#endif
