#ifndef LOANUPDATE_HPP
#define LOANUPDATE_HPP

#include "NRand.h"
#include "LoanDef.h"
#include "NextStepCalc.h"
#include "Amort.h"
#include "SevCalc.h"
#include "ReadData.hpp"

using namespace std;

//Updates the delinquency score by using different weight on different delinquency statuses
//return updated delinquency score
double pct_deno(int age, char status, double pct);

//calculates the target interest rate to bring DTI down to 31% for HAMP by using the Newton method
//and returns interest rate calculated
double solve_int(LoanDef &loan);
//HAMP function based on given set of criteria which modifies the interest rate, current status and DTI to 31
//void LoanHAMP(LoanDef &loan);

//the corresponding loan status update function with modification
void loanmod_next_status(LoanDef &loan, double libor, double hpa, double unemploy, double mort_rate,
                 double orig_bal, double orig_ltv, double orig_nltv,
                 unordered_map<string, vector<double> > &sev_coeff, 
                 NextStepCalcML &nsc);

#endif
