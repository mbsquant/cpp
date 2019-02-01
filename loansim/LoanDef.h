#ifndef LOANDEF_H
#define LOANDEF_H

#include <string>
#include <vector>
#include <iostream>
#include "amort_data.h"
#include "NRand.h"
#include <cfloat>
#include "sse_mathfun.h"
#include <unordered_map>
//#include "LoanUpdate.hpp"

class NextStepCalcML;
class Assumption;

using namespace std;
//header file that defines the loan definition class, with get and set functions and a member function for reading all the loans from a file
class LoanDef {
    typedef vector<double> loanvec;
    friend class NextStepCalcML;
    friend void loanmod_next_status(LoanDef&, double, double, double, double, double, double, double,
                 unordered_map<string, vector<double> >&, 
                 NextStepCalcML &);
    friend void Deal_Sim(vector<LoanDef> &, Assumption &, unordered_map<string, vector<double> > &, 
        unsigned int, NextStepCalcML &, vector<vector<vector<double> > > &, unsigned int, unsigned int);

private:
	string NAME;		//loan ID	
	string PROD_TYPE;	//product type
	string LOC;			//location
	double FIRST_RATE;	//first reset rate
	char PROP_TYPE;		//property type
	char OCCUPANCY;		//occupancy
	char PURPOSE;		//purpose
	char DOCUMENT;		//documentation type	
	unsigned int LIEN;			//lien position
	unsigned int TERM;		//loan term
	double INIT_RATE;	//initial spread
	double principal;	//principal
	char STAT;			//current status
	unsigned int FICO;		//fico score
	double LTV;			//effective combined loan-to-value
	double INT_RATE;	//initial interest rate
	double DTI;			//debt to income ratio
	unsigned int AGE;			//loan age
	amort_data amor;	//amortization data
	double prin_pay;	//principal payment
	double int_pay;		//interest payment
	double prin_acc;	//principal accumulated
	double int_acc;		//interest accumulated
	double def_amt;		//default amount
	string CBSA;		//CBSA code
	double deq_hist;	//delinquent history, in percentage
	int deq_month;	    //months in delinquency 

	double d_ltv;		//change of loan-to-value ratio
	double reo_mean;	//REO mean
	double reo_vol;		//REO volatility
	int reo_rem;		//remaining REO time

	double prep_inc;	//prepayment incentive
	int pmt_shock;		//time to payment shock

	double un_rate;		//unemployment rate
	int SEASON;			//seasoning or age of the loan

	double init;		//loan modification prepay record
	double NLTV;		// this is eff_ltv
	int deq_m2;			//this is to record delinquent months

	int type;			//type
		
	int reo_time;		//auxiliary variable
	string pmtHistory;  //payment history string
	int mod_flag;		//loan modification flag
	bool ltv_flag;
    loanvec res;
    
public:
	//constructor
	
	LoanDef(string name_x, string prod, string loc_r, double first_r, char prop, char occ, char pur, char doc, 
	unsigned int lien_x, unsigned int n_term, double init, double prin, char st, unsigned int fc, double ltv_x, double int_r, double dti_x, unsigned int age_x,
	double a_tr, double a_tt, double a_ic, double a_margin, double a_rf, double a_initcap, double a_ped_cap, double a_life_cap, double a_lflr, 
	unsigned int am_tm, unsigned int io_tm, double n_l, double rct, double p_pp, double p_ip, double p_pa, double p_ia, double d_amt, string cbsa_cd, double dh, int dm,
	double i_dltv, double i_reomean, double i_reovol, int i_reorem, double i_prepinc, int i_sk, double i_unrate, unsigned int i_season, unsigned int i_init, double i_nltv_x, string ph1, string ph2); 
	

	/* gets and sets function of class */
	inline void change_STAT(char statx){
		STAT = statx; 
	}

	inline bool get_ltv_flag(){
		return ltv_flag;
	}

	inline void set_ltv_flag(bool fg){
		ltv_flag = fg; 
	}

	//return loan status
	inline char get_STAT(){
		return STAT; 
	}

	//return CBSA code
	inline string get_CBSA(){
		return CBSA; 
	}

	//change delinquent month record
	inline void change_deq2() {
		deq_m2=deq_m2+1;
	}

	//change delinquent month record
	inline void change_deq2(int ss) {
		deq_m2=ss;
	}

	//get occupancy
	inline char get_OCC() {
		return OCCUPANCY;
	}

	//return delinquent month record
	inline int get_deq2() {
		return deq_m2;
	}

	//change fico score
	inline void change_FICO(unsigned int ficox){
		FICO = ficox;
	}

	//return fico score
	inline unsigned int get_FICO(){
		return FICO;
	}

	//change loan-to-value 
	inline void change_LTV(double ltvx){
		LTV = ltvx;
	}

	//depleted
	inline void change_NLTV(double ntvx){
		NLTV = ntvx;
	}

	//return loan-to-value
	inline double get_LTV(){
		return LTV;
	}

	//depleted
	inline double get_NLTV(){
		return NLTV;
	}

	//return loan age
	inline unsigned int get_AGE(){
		return AGE;
	}

	//increment loan age
	inline void add_AGE(){
		AGE++;
	}

	//change loan age
	inline void change_AGE(unsigned int ag){
		AGE = ag;
	}

	//return debt-to-income
	inline double get_DTI(){
		return DTI;
	}

	//return loan location
	inline string get_LOC(){
		return LOC;
	}

	//change debt-to-income
	inline void change_DTI(double dtix){
		DTI = dtix;
	}

	//return loan balance
	inline double get_BAL() {
		return principal;
	}

	//return loan term
	inline unsigned int get_TERM() {
		return TERM;
	}

	//return interest rate
	inline double get_INT_RATE() {
		return INT_RATE;
	}

	//change interest rate
	inline void change_INT_RATE(double ir) {
		INT_RATE = ir;
	}

	//change initial rate
	inline void change_INIT_RATE(double ir) {
		INIT_RATE = ir;
	}

	//change loan balance
	inline void change_BAL(double bal) {
		principal = bal;
	}

	//change principal payment
	inline void change_prin_pay(double pp) {
		prin_pay = pp;
	}

	//change interest payment
	inline void change_int_pay(double ip){
		int_pay = ip;
	}

	//return principal payment
	inline double get_prin_pay() {
		return prin_pay;
	}

	//return interest payment
	inline double get_int_pay() {
		return int_pay;
	}

	//return amortization class
	inline amort_data get_amor() {
		return amor;
	}

	//return first reset rate
	inline double get_F_RATE() {
		return FIRST_RATE;
	}

	//return product type code
	inline string get_PROD_TYPE() {
		return PROD_TYPE;
	}

	//return property type code
	inline char get_PROP_TYPE() {
		return PROP_TYPE;
	}

	//return interest payment accumulated
	inline double get_int_acc() {
		return int_acc;
	}

	//return principal payment accumulated
	inline double get_prin_acc() {
		return prin_acc;
	}

	//change interest payment accumulated
	inline void change_int_acc(double ia) {
		int_acc = ia;
	}

	//change principal payment accumulated
	inline void change_prin_acc(double pa) {
		prin_acc = pa;
	}

	//change default amount
	inline void change_def_amt(double da) {
		def_amt=da;
	}
	
	//return default amount
	inline double get_def_amt() {
		return def_amt;
	}
		
	//return loan purpose
	inline char get_PURPOSE() {
		return PURPOSE;
	}

	//return lien position
	inline unsigned int get_LIEN() {
		return LIEN;
	}

	//return number of months in delinquency
	inline int get_deq_m() {
		return deq_month;
	}

	//change number of months in delinquency
	inline void change_deq_m(int dd) {
		deq_month=dd;
	}
	
	//return delinquency history score
	inline double get_deq_hist() {
		return deq_hist;
	}

	//change delinquency history score
	inline void change_deq_hist(double dp) {
		deq_hist = dp;
	}

	//return the delta of loan-to-value
	inline double get_dltv() {
		return d_ltv;
	}

	//change the delta of loan-to-value
	inline void change_dltv(double dl) {
		d_ltv = dl;
	}

	//return prepay incentive
	inline double get_prep_inc() {
		return prep_inc;
	}

	//change prepay incentive
	inline void change_prep_inc(double dl) {
		prep_inc = dl;
	}

	//return REO remaining term
	inline int get_reo_rem() {
		return reo_rem;
	}

	//decrement REO remaining term
	inline void change_reo_rem() {
		reo_rem = reo_rem - 1;
	}

	//return mean of REO remaining term
	inline double get_reo_mean() {
		return reo_mean;
	}

	//return volatility of REO remaining term
	inline double get_reo_vol() {
		return reo_vol;
	}

	//return payment shock
	inline int get_pmt_shock() {
		return pmt_shock;
	}

	//change payment shock
	inline void change_pmt_shock() {
		pmt_shock = pmt_shock - 1;
	}

	//reset payment shock
	inline void reset_pmt_shock(){
		pmt_shock = 0;
	}

	//return unemployment rate
	inline double get_un_rate() {
		return un_rate;
	}

	//change unemployment rate
	inline void change_un_rate(double ur) {
		un_rate = ur;
	}

	//increment season
	inline void change_SEASON() {
		SEASON = SEASON%12+1;
	}

	//return current season
	inline unsigned int get_SEASON() {
		return SEASON;
	}

	//return the identification number of the loan
	inline string get_NAME() {
		return NAME;
	}
	

	//set of REO remaining time by simulation based on historical mean and volatility
	inline void set_reo_rem() {
		int res;
		res = int(exp_ps(nrand(reo_mean, reo_vol)));
		reo_rem = res-1;    // old
		//reo_rem = res - reo_rem;
		//if (reo_rem<1) reo_rem=1;

	}

	//change loan modification prepay record
	inline void change_init(double in) {

		init = in;
	}

	//change property type
	inline void change_PROD_TYPE(string t) {
		PROD_TYPE = t;
	}

	//return loan modification prepay record
	inline double get_init() {
		return init;
	}

	//return loan type
	inline int get_type() {
		return type;
	}

	//return REO timeline
	inline int get_reo_time() {
		return reo_time;
	}

	//return loan modification flag
	inline int get_mod_flag() {
		return mod_flag;
	}

	//change REO timeline
	inline void change_reo_time(int x) {
		reo_time = x;
	}

	inline string get_pmtHistory(){
		return pmtHistory;
	}

	inline void change_pmtHistory(string tmp){
		pmtHistory = tmp;
	}

	//change loan modification flag
	inline void change_mod_flag(int x) {
		mod_flag = x;
	}

	void update_vec();

};

# endif
