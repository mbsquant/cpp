#ifndef AMORT_H
#define AMORT_H

#include "LoanDef.h"
#include "amort_data.h"
#include <cmath>
#include <algorithm>
#include "LoanUpdate.hpp"

using namespace std;

// this header file defines amortization class, which is used for the monthly amortization calculations
class amort{
public:
	//Calculates the payment for a loan based on fixed interest rate, term and principal.
	//return the payment
	double f_PMT(double rate, int term, double pv){
		// case when rate is zero
		if(rate==0) return 0.;
		else return pv*(rate/12 + rate/12/(pow(1+rate/12, term)-1.0));
	}

	//amortization for a fixed rate mortgage
	//return a vector of interest payment, principal payment, ending balance and mortgage rate
	vector<double> amort_FRM(unsigned int age, double balance, unsigned int term, double rate, unsigned int io, unsigned int xterm) {
		//return the ending balance, principal payment, and interest payment
		double interest;
		double principal;
		double payment;
		double end_bal;
		vector<double> res(4);

		//when loan still in interest only period
		if(age<=io) {
			payment = balance*rate/12.0 ;
			principal = 0;
			interest = payment;
			end_bal = balance;

			/* old code start
			//res.push_back(interest);
			//res.push_back(principal);
			//res.push_back(end_bal);
			//res.push_back(rate);
			old code end */

			res[0]=interest;
			res[1]=principal;
			res[2]=end_bal;
			res[3]=rate;
		}

		//when the loan is still alive
		else if(age<term) {
			interest = balance*rate/12.0;
			if(age <= xterm){
				payment = f_PMT(rate, xterm-age+1, balance); 
				principal = payment-interest;
				end_bal = balance-principal;
			}
			else {
				payment = balance + interest;
				principal = balance;
				end_bal = 0.;
			}

			/* old code start
			//res.push_back(interest);
			//res.push_back(principal);
			//res.push_back(end_bal);
			//res.push_back(rate);
			old code end */

			res[0]=interest;
			res[1]=principal;
			res[2]=end_bal;
			res[3]=rate;
		}

		//at the end of the loan term, the ending balance is zero
		else if(age==term) {
			
			interest = balance*rate/12.0;
			payment = interest + balance;
			principal = balance;
			/* old code start
			//res.push_back(interest);
			//res.push_back(balance);
			//res.push_back(0);
			//res.push_back(rate);
			old code end */

			res[0]=interest;
			res[1]=principal;
			res[2]=0;
			res[3]=rate;
		}

		//another abnormal term terminates the simulation
		else {
			//res.push_back(0);
			//res.push_back(0);
			//res.push_back(0);
			//res.push_back(rate);

			res[0]=0;
			res[1]=0;
			res[2]=0;
			res[3]=rate;
		}
		return res;

	}

	//amortization for adjustable rate mortgage
	//return a vector of interest payment, principal payment, ending balance and mortgage rate

	vector<double> amort_ARM(double balance, double int_rate, unsigned int term, unsigned int xterm, unsigned int age, double teaser_rate, double teaser_term, double init_coupon, 
		double margin, double first_rate, double reset_freq, double init_cap, double perd_cap, double life_cap, double life_flr, double libor, unsigned int io){

		//return interest payment, principal payment, ending balance and current mortgage rate
		double interest;
		double principal;
		double payment;
		double mid_1, mid_2;
		double cur_rate; // current interest rate used

		//if the loan has any teaser term 
		if(age<=teaser_term) {
			cur_rate = teaser_rate;
		}

		//if the loan has passed the teaser term and has not been reset
		else if(age>teaser_term && age<=first_rate) {
			cur_rate = init_coupon;
		}

		//if loan has been reset and is not in its reset period
		else if(age>first_rate && (age-1)%int(reset_freq) > 0) {
			cur_rate = int_rate;
		}

		//if the loan has been reset and is right in its reset period, calculate interest rate by 
		//the minimum of two formulas
		else if(age>first_rate && (age-1)%int(reset_freq) ==0) {
			mid_1 = init_coupon + init_cap+perd_cap*(int((age-first_rate-1)/reset_freq));
			mid_2 = libor+margin;
			if (mid_1>mid_2) {cur_rate = mid_2;}
			else cur_rate = mid_1;
		}

		//TODO: check

		else {
			cur_rate = int_rate;
		}

		//apply the cap and floor to the interest rate
		if(cur_rate<life_flr) {cur_rate=life_flr;}
		//TODO: check
		else if(cur_rate>life_cap) {cur_rate = life_cap;}

		interest = cur_rate/12.0*balance;

		//calculation for interest only period
		if (age<=io) { 
            payment = interest; 
        }
		else if(age>term){
			interest = 0;
			payment=0;
		} 
        //calculation for last month of loan term
        else if(age==term) {
			payment = balance + interest;
			principal = balance;
		}
		//calculation of payment, principal
        // implicitly age < term
		else{ 
			//payment = f_PMT(cur_rate, xterm-age+1, balance); 
			
			if(age <= xterm){
				payment = f_PMT(cur_rate, xterm-age+1, balance); 
			}
			else {
				payment = balance + interest;
				principal = balance;
			}
			/*
			if(!IsFiniteNumber(payment)){
				cout << "Check payment function" << endl;
				cout << cur_rate << "|" << xterm << "|" << age << "|" << balance << "|" << term << endl;
			}
			*/
		}

		principal = payment - interest;

		vector<double> resx(4);

		/* old code start
		//resx.push_back(interest);
		//resx.push_back(principal);
		//resx.push_back(balance-principal);  //this is ending balance
		//resx.push_back(cur_rate);
		old code end */

		resx[0]=interest;			//interest payment
		resx[1]=principal;			//principal payment
		resx[2]=balance-principal;  //ending balance
		resx[3]=cur_rate;			//current interest rate

		return resx;
	 }

	// call of function amort_ARM with some automatic assignment of parameters
	//return a vector of interest payment, principal payment, ending balance and mortgage rate
	vector<double> amort_ARM_X(double balance, double int_rate, unsigned int term, unsigned int age, double first_rate, double libor, amort_data am) {
		double teaser_rate, teaser_term, init_coupon, margin, reset_freq, init_cap, perd_cap, life_cap, life_flr;
		unsigned int xterm, io;
		teaser_rate = am.teaser_rate;
		teaser_term = am.teaser_term;
		init_coupon = am.init_coupon;
		margin = am.margin;
		reset_freq = am.reset_freq;
		init_cap = am.init_cap;
		perd_cap = am.perd_cap;
		life_cap = am.life_cap;
		life_flr = am.life_flr;
		xterm = am.amort_term;
		io = am.io;
		
		//call amort_ARM function
		vector<double> res = amort_ARM(balance, int_rate, term, xterm, age, teaser_rate, teaser_term, init_coupon, margin, 
			first_rate, reset_freq, init_cap, perd_cap, life_cap, life_flr, libor, io);
		return res;
	}

	//amortization for negative amortization mortgage
	//return a vector of interest payment, principal payment, ending balance and mortgage rate
	vector<double> amort_NEG(double balance, double int_rate, unsigned int term, unsigned int xterm, unsigned int age, double teaser_rate, double teaser_term, double init_coupon, 
		double margin, double first_rate, double reset_freq, double init_cap, double perd_cap, double life_cap, double life_flr, double libor, unsigned int io, 
		double neg_limit, double mini_pmt){
			//return interest payment, principal payment, ending balance and current mortgage rate

		double interest;
		double principal;
		double payment;
		double mid_1, mid_2;
		double req_pmt;

		double cur_rate; // current rate used

		//the loan is still in its teaser term
		if(age<=teaser_term) {
			cur_rate = teaser_rate;
		}

		//the teaser term has expired but the loan has not had its rate reset
		else if(age>teaser_term && age<=first_rate) {
			cur_rate = init_coupon;
		}

		//the loan has been reset and is not in its reset period
		else if(age>first_rate && (age-1)%int(reset_freq) > 0) {
			cur_rate = int_rate;
		}

		//if the loan has been reset and is right in its reset period, calculate the interest rate by 
		//the minimum of two formulas
		else if(age>first_rate && (age-1)%int(reset_freq) ==0) {
			mid_1 = init_coupon + init_cap+perd_cap*(int((age-first_rate-1)/reset_freq));
			mid_2 = libor+margin;
			if (mid_1>mid_2) {cur_rate = mid_2;}
			else cur_rate = mid_1;
		}

        else {
            cur_rate = int_rate;
        }

		//apply the cap and floor on the interest rate
		if(cur_rate<life_flr) {cur_rate=life_flr;}
		else if(cur_rate>life_cap) {cur_rate = life_cap;}

		interest = cur_rate/12.0*balance;

		//now calculate the required payment
		if (age<=io) {req_pmt = interest;}

		//loan ends
		else if(age>term) {
			req_pmt=0;
			interest = 0;
		} 

		//last month of loan term
		else if(age==term) {
			req_pmt = balance + interest;
			principal = balance;
		}

		//required payment calculation
		else  { 
			if(age <= xterm){
				req_pmt = f_PMT(cur_rate, xterm-age+1, balance); 
			}
			else {
				req_pmt = balance + interest;
			}
		}

		principal = req_pmt - interest;

		//the loan is in its negative amortization period, 
		//its payment is the minimum payment and its balance increases by the required payment minus the minimum payment
		if (age<60 && balance<neg_limit) {
			payment = mini_pmt;
			balance += interest - mini_pmt;
			principal = max(0.0, mini_pmt-interest);
			interest = min(interest, payment);
		}

		//after the negative amortization period, the payment is normal
		else {
			payment = req_pmt;
			principal = payment - interest;
		}

		vector<double> resx(4);

		/*old code start
		//resx.push_back(interest);
		//resx.push_back(principal);
		//resx.push_back(balance-principal);  //this is ending balance
		//resx.push_back(cur_rate);
		old code end*/

		resx[0]=interest;
		resx[1]=principal;
		resx[2]=balance-principal;
		resx[3]=cur_rate;
		
		return resx;
	
	}

	//calls the function amort_NEG with some automatic assignment of parameters
	//returns a vector of interest payment, principal payment, ending balance and mortgage rate
	vector<double> amort_NEG_X(double balance, double int_rate, unsigned int term, unsigned int age, double first_rate, double libor, amort_data am) {
		double teaser_rate, teaser_term, init_coupon, margin, reset_freq, init_cap, perd_cap, life_cap, life_flr, neg_limit, mini_pay;
		unsigned int xterm, io;
		teaser_rate = am.teaser_rate;
		teaser_term = am.teaser_term;
		init_coupon = am.init_coupon;
		margin = am.margin;

		//get reset_frequency if loan has this data, else assign to one
		if(am.reset_freq>0) {
			reset_freq = am.reset_freq;
		}
		else reset_freq=1;

		init_cap = am.init_cap;
		perd_cap = am.perd_cap;
		life_cap = am.life_cap;
		life_flr = am.life_flr;
		xterm = am.amort_term;
		io = am.io;
		neg_limit = am.neg_limit;
		mini_pay = am.mini_pay;

		//reset minimal payment by periodical cap every year
		if(age%12==0) {
			am.change_mini_pay(mini_pay*(1+am.perd_cap));
		}
		
		//call the amort_NEG function
		vector<double> res = amort_NEG(balance, int_rate, term, xterm, age, teaser_rate, teaser_term, init_coupon, margin, 
			first_rate, reset_freq, init_cap, perd_cap, life_cap, life_flr, libor, io, neg_limit, mini_pay);
		return res;
	}

};

#endif

