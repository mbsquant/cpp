#ifndef SEVCALC_H
#define SEVCALC_H

#include "LoanDef.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

using namespace std;

//severity calculation class, which runs the calculation based on a logistic multinomial regression

class SevCalc{
public:
	//return the severity of the loan
	double sev(char status_b4, LoanDef &loan, unordered_map<string, vector<double> > &sev_coeff) {
		double state_coeff=0;        //coefficient for state 
		double pp_coeff=0;			 //coefficient for loan purpose 
		double amt_coeff=0;			 //coefficient for loan balance
		double lien_coeff=0;		 //coefficient for lien position
		double inter_coeff=0;		 //coefficient for intercept
		string AMT_s = "AMT";
		double res = 0;
		int AMT = 0;
		AMT = int(loan.get_BAL()/50000.0);
		if(AMT>22) {AMT=30;}
		if(AMT<0) {AMT=0;}
		std::stringstream out;
		out << AMT;
		AMT_s += out.str();
		
		//mapping the type of the loan into 0, 1, 2
		int type;
		type = loan.get_type()-1;
		bool debug_sev = false;
		// type 0 subprime, 1 prime, 2 alt-a
		
		/* calculate the severity based on previous status, location, lien position, loan amount, purpose, interest rate,
		age, loan-to-value ratio, fico and reo remaining time, mapped to corresponding set of regression coefficients */

		//previous status is '9'
		if(status_b4=='9') {
				if(sev_coeff.find(loan.get_LOC())==sev_coeff.end()) {
					state_coeff=sev_coeff.find("AA")->second[0+3*type];
				}
				else {
					state_coeff=sev_coeff.find(loan.get_LOC())->second[0+3*type];
				}	

				
				inter_coeff = sev_coeff.find("INTER")->second[0+3*type];
				amt_coeff=sev_coeff.find(AMT_s)->second[0+3*type];
				if(loan.get_PURPOSE()=='2') {
					pp_coeff = sev_coeff.find("PP2")->second[0+3*type];
				}
				else if(loan.get_PURPOSE()=='3') {
					pp_coeff = sev_coeff.find("PP3")->second[0+3*type];
				}
				if(loan.get_LIEN()==2){
					lien_coeff = sev_coeff.find("LIEN")->second[0+3*type];
				}
				res = inter_coeff+state_coeff+amt_coeff+pp_coeff+lien_coeff+
					loan.get_AGE()*sev_coeff.find("AGE")->second[0+3*type]+
					loan.get_LTV()*sev_coeff.find("LTV")->second[0+3*type]+
					loan.get_FICO()*sev_coeff.find("FICO")->second[0+3*type];
				if(debug_sev){
				cout << "|SEVCOF|" << inter_coeff << "|" << state_coeff << "|" << amt_coeff << "|" << pp_coeff << "|" << 
						lien_coeff << "|" << sev_coeff.find("AGE")->second[0+3*type] << "|" << 
						sev_coeff.find("LTV")->second[0+3*type] << "|" << sev_coeff.find("FICO")->second[0+3*type] << "\n";
				cout << loan.get_NAME() << "|9|1|1|1|1|1|" << loan.get_AGE() << "|" << loan.get_NLTV() << "|" <<  loan.get_FICO() << "|" << res << "|\n";
				}
		}

		//previous status is 'R'
		else if(status_b4=='R') {
				if(sev_coeff.find(loan.get_LOC())==sev_coeff.end()) {
					state_coeff=sev_coeff.find("AA")->second[2+3*type];
				}
				else {
					state_coeff=sev_coeff.find(loan.get_LOC())->second[2+3*type];
				}	
				inter_coeff = sev_coeff.find("INTER")->second[2+3*type];
				amt_coeff=sev_coeff.find(AMT_s)->second[2+3*type];
				if(loan.get_PURPOSE()=='2') {
					pp_coeff = sev_coeff.find("PP2")->second[2+3*type];
				}
				else if(loan.get_PURPOSE()=='3') {
					pp_coeff = sev_coeff.find("PP3")->second[2+3*type];
				}
				if(loan.get_LIEN()==2){
					lien_coeff = sev_coeff.find("LIEN")->second[2+3*type];
				}
				res = inter_coeff+state_coeff+amt_coeff+pp_coeff+lien_coeff+
					loan.get_AGE()*sev_coeff.find("AGE")->second[2+3*type]+
					loan.get_LTV()*sev_coeff.find("LTV")->second[2+3*type]+
					loan.get_FICO()*sev_coeff.find("FICO")->second[2+3*type]
					+loan.get_reo_time()*sev_coeff.find("REOTIME")->second[2+3*type];
				if(debug_sev){
					cout << "|SEVCOF|" << inter_coeff << "|" << state_coeff << "|" << amt_coeff << "|" << pp_coeff << "|" << 
						lien_coeff << "|" << sev_coeff.find("AGE")->second[2+3*type] << "|" << 
						sev_coeff.find("LTV")->second[2+3*type] << "|" << sev_coeff.find("FICO")->second[2+3*type] << "\n";
					cout << loan.get_NAME() << "|R|1|1|1|1|1|" << loan.get_AGE() << "|" << loan.get_NLTV() << "|" <<  loan.get_FICO() << "|" << res << "|\n";
				}
			}

		//previous status is 'F'
		else if(status_b4=='F') {
				if(sev_coeff.find(loan.get_LOC())==sev_coeff.end()) {
					state_coeff=sev_coeff.find("AA")->second[1+3*type];
				}
				else {
					state_coeff=sev_coeff.find(loan.get_LOC())->second[1+3*type];
				}
				inter_coeff = sev_coeff.find("INTER")->second[1+3*type];
				amt_coeff=sev_coeff.find(AMT_s)->second[1+3*type];
				if(loan.get_PURPOSE()=='2') {
					pp_coeff = sev_coeff.find("PP2")->second[1+3*type];
				}
				else if(loan.get_PURPOSE()=='3') {
					pp_coeff = sev_coeff.find("PP3")->second[1+3*type];
				}

				if(loan.get_LIEN()==2){
					lien_coeff = sev_coeff.find("LIEN")->second[1+3*type];
				}
				res = inter_coeff+state_coeff+amt_coeff+pp_coeff+lien_coeff+
					loan.get_AGE()*sev_coeff.find("AGE")->second[1+3*type]+
					loan.get_LTV()*sev_coeff.find("LTV")->second[1+3*type]+
					loan.get_FICO()*sev_coeff.find("FICO")->second[1+3*type];
				if(debug_sev){
				cout << "|SEVCOF|" << inter_coeff << "|" << state_coeff << "|" << amt_coeff << "|" << pp_coeff << "|" << 
						lien_coeff << "|" << sev_coeff.find("AGE")->second[1+3*type] << "|" << 
						sev_coeff.find("LTV")->second[1+3*type] << "|" << sev_coeff.find("FICO")->second[1+3*type] << "\n";
				cout << loan.get_NAME() << "|F|1|1|1|1|1|" << loan.get_AGE() << "|" << loan.get_NLTV() << "|" <<  loan.get_FICO() << "|" << res << "|\n";
				}
		}
    //if(loan.get_NLTV()<120) res += 0.15;
	if(res>1) res=1;
	/*
	if(res<0 || isNA(res)) {
		res=0;
	}
	*/
	return res;
	}
};



#endif
