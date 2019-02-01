#include "LoanDef.h"
#include <cfloat>
#include "sse_mathfun.h"

using namespace std;

static const double weight[60] = {4.59514,4.48169,4.37104,4.26311,4.15786,4.0552,3.95508,3.85743,3.76219,3.6693,3.5787,3.49034,3.40417,3.32012,3.23814,3.15819,3.08022,3.00417,2.92999,2.85765,2.7871,2.71828,2.65117,2.58571,2.52187,2.4596,2.39888,2.33965,2.28188,2.22554,2.17059,2.117,2.06473,2.01375,1.96403,1.91554,1.86825,1.82212,1.77713,1.73325,1.69046,1.64872,1.60801,1.56831,1.52959,1.49182,1.45499,1.41907,1.38403,1.34986,1.31653,1.28403,1.25232,1.2214,1.19125,1.16183,1.13315,1.10517,1.07788,1.05127};

static const double total[60] = {1.05127,2.12916,3.23433,4.36747,5.52931,6.72056,7.94196,9.19428,10.4783,11.7948,13.1447,14.5287,15.9478,17.4028,18.8946,20.4242,21.9925,23.6005,25.2492,26.9397,28.673,30.4501,32.2722,34.1405,36.056,38.02,40.0338,42.0985,44.2155,46.3861,48.6116,50.8935,53.2332,55.632,58.0917,60.6135,63.1992,65.8504,68.5687,71.3558,74.2134,77.1434,80.1476,83.2278,86.386,89.6241,92.9443,96.3484,99.8388,103.417,107.087,110.849,114.706,118.661,122.717,126.875,131.138,135.509,139.99,144.585}; 

//reading from file
LoanDef::LoanDef(
	string name_x, 
	string prod, 
	string loc_r, 
	double first_r, 
	char prop, 
	char occ, 
	char pur, 
	char doc, 
	unsigned int lien_x, 
	unsigned int n_term, 
	double init, 
	double prin, 
	char st, 
	unsigned int fc, 
	double ltv_x, 
	double int_r, 
	double dti_x, 
	unsigned int age_x,
	double a_tr, 
	double a_tt, 
	double a_ic, 
	double a_margin, 
	double a_rf, 
	double a_initcap, 
	double a_ped_cap, 
	double a_life_cap, 
	double a_lflr, 
	unsigned int am_tm, 
	unsigned int io_tm, 
	double n_l, 
	double rct, 
	double p_pp, 
	double p_ip, 
	double p_pa, 
	double p_ia, 
	double d_amt,  
	string cbsa_cd, 
	double dh, 
	int dm,
	double i_dltv, 
	double i_reomean, 
	double i_reovol, 
	int i_reorem, 
	double i_prepinc, 
	int i_sk, 
	double i_unrate, 
	unsigned int i_season, 
	unsigned int i_init, 
	double i_nltv_x,
	string ph1,
	string ph2){
	NAME = name_x;
	PROD_TYPE = prod;
	LOC = loc_r;
	FIRST_RATE = first_r;
	PROP_TYPE = prop;
	OCCUPANCY = occ;
	PURPOSE = pur;
	DOCUMENT = doc; 
	LIEN = lien_x;
	TERM = n_term;
	INIT_RATE = init;
	principal = prin;
	STAT = st;
	FICO = fc;
	LTV = ltv_x;
	INT_RATE = int_r;
	DTI = dti_x;
	AGE = age_x;

	amor.teaser_rate=a_tr;
	amor.teaser_term=a_tt;
	amor.init_coupon=a_ic;
	amor.margin=a_margin;
	amor.reset_freq=a_rf;
	if(a_rf<1) amor.reset_freq=1;
	amor.init_cap=a_initcap;
	amor.perd_cap=a_ped_cap;
	amor.life_cap=a_life_cap;
	amor.life_flr=a_lflr;
	amor.amort_term=am_tm;
	amor.io=io_tm;
	amor.neg_limit = n_l;
	amor.mini_pay = rct;

	prin_pay = p_pp;
	int_pay=p_ip;
	prin_acc = p_pa;
	int_acc = p_ia;
	def_amt = d_amt;
	CBSA = cbsa_cd;
	deq_hist = dh;
	deq_month=dm;

	d_ltv=i_dltv;
	reo_mean = i_reomean;
	reo_vol = i_reovol;
	reo_rem = i_reorem;
	prep_inc = i_prepinc;
	pmt_shock = i_sk;

	un_rate = i_unrate;
	SEASON = i_season;

	init = 0.0;
	//cout << "From Loan: " << init << endl;
	NLTV = i_nltv_x;
	deq_m2=max(dm,0);

	type=i_init;
	reo_time  = max(0, i_reorem);

	if(ph1.length() > 60) ph1 = ph1.substr(ph1.length()-60, 60);
	if(ph2.length() > 60) ph2 = ph2.substr(ph2.length()-60, 60);

	pmtHistory = "";
	
	if(ph1.length()<1) ph1 = st;
	if(ph2.length()<1) ph2 = st;
	

	//if(ph1=="") ph1 = st;
	//if(ph2=="") ph2 = st;

	for(size_t sz=0; sz!=ph1.length(); ++sz){
		if(ph1.at(sz)=='F' || ph1.at(sz)=='R' || ph1.at(sz)=='B') pmtHistory += ph1.at(sz);
		else pmtHistory += ph2.at(sz);
	}

	mod_flag = 0;
	ltv_flag = false;
	
    res.reserve(27);
    res.resize(27);
	//PROP_TYPE
	if(PROP_TYPE=='4'||PROP_TYPE=='7'){
		res[0]=1;
		res[1]=0;
	}
	else if (PROP_TYPE=='2'||PROP_TYPE=='5'||PROP_TYPE=='6'||PROP_TYPE=='M'){
		res[0]=0;
		res[1]=1;
	}
	else {
		res[0]=0;
		res[1]=0;
	}
	
	//PROD_TYPE
	if(PROD_TYPE.at(0)=='2') {
		res[2]=1;
		res[3]=0;
	}
	else if(PROD_TYPE.at(0)=='5') {
		res[2]=0;
		res[3]=1;
	}
	else {
		res[2]=0;
		res[3]=0;
	}

	//OCCUPANCY
	if(OCCUPANCY=='2') {
		res[4]=1;
		res[5]=0;
	}

	else if(OCCUPANCY=='3') {
		res[4]=0;
		res[5]=1;
	}

	else{
		res[4]=0;
		res[5]=0;
	}

	//PURPOSE
	if(PURPOSE=='2') {
		res[6]=1;
		res[7]=0;
	}

	else if(PURPOSE=='3') {
		res[6]=0;
		res[7]=1;
	}

	else {
		res[6]=0;
		res[7]=0;

	}

	//DOCUMENT
	if (DOCUMENT=='2') {
		res[8]=1;
	}

	else {
		res[8]=0;
	}

	// shock
	//res.push_back(abs(pmt_shock));
	res[9]=abs(pmt_shock);

	//TERM
	//res.push_back(TERM);
	res[10]=TERM;
	//res.push_back(LTV);
	res[11]=LTV;

	if(d_ltv>5.0) {
		res[12]=5.0;
	}
	else if (d_ltv<-10.0) {
		res[12]=-10.0;
	}
	else {
		res[12]=d_ltv;
	}

	res[13]=FICO;
	
	res[14]=DTI;

	res[15]=INT_RATE;

	res[16]=INIT_RATE;

	res[17]=AGE;
	
    //NEW: deq_hist part
#ifdef NO_DEQHIST_UPDATE
    res[18]=deq_hist;
	
#else
	double wt = 0., sum = 0.;
    unsigned int pmtlength = pmtHistory.length();
	for(unsigned int sz=0; sz<pmtlength; ++sz){
		switch(pmtHistory[sz]){
			case '0':
				wt = 1;
                break;
            case '1':
                wt = 2;
                break;
            case '2':
                wt = 4;
                break;
            case '3':
            case '4':
            case '5':
            case 'F':
            case 'R':
            case 'B':
				wt = 6;
                break;
            default:
                wt = -1;
                break;
		}
        //sum += wt*exp_ps((pmtHistory.length() - sz + 1)/40.0);
        sum += wt*weight[60-pmtlength+sz];
	}
	res[18] = sum/total[pmtlength-1]-1.;
#endif
	
	//TODO: what if the payment string is empty?

	res[19]=un_rate;

	res[20]=prep_inc;

	res[21]=principal/100000.0;

	if (SEASON<4 && SEASON!=0) {
		res[22]=0;
		res[23]=0;
		res[24]=0;
	}
	else if (SEASON<7 && SEASON!=0) {
		//res.push_back(1);
		//res.push_back(0);
		//res.push_back(0);
		res[22]=1;
		res[23]=0;
		res[24]=0;
	}
	else if (SEASON<10 && SEASON!=0) {
		res[22]=0;
		res[23]=1;
		res[24]=0;
	}
	else {
		res[22]=0;
		res[23]=0;
		res[24]=1;
	}
	
    //res[25]=100*amor.init_coupon;
    res[25] = amor.init_coupon;
    res[26]=TERM>200?1:0;
}

//get vector from Loans for calculation of transition probabilities, such as product, property, purpose type code
void LoanDef::update_vec(){

	// shock
	//res.push_back(abs(pmt_shock));
	res[9]=abs(pmt_shock);

	res[11]=LTV;

	if(d_ltv>5.0) {
		res[12]=5.0;
	}
	else if (d_ltv<-10.0) {
		res[12]=-10.0;
	}
	else {
		res[12]=d_ltv;
	}

	res[15]=INT_RATE;

	res[17]=AGE;

    //NEW: deq_hist part
#ifdef NO_DEQHIST_UPDATE
    res[18]=deq_hist;
	//res[18]=0.0;
	
#else
	double wt = 0., sum = 0.;
    unsigned int pmtlength = pmtHistory.length();
	for(unsigned int sz=0; sz<pmtlength; ++sz){
		switch (pmtHistory[sz]){
			case '0':
				wt = 1;
                break;
            case '1':
                wt = 2;
                break;
            case '2':
                wt = 4;
                break;
            case '3':
            case '4':
            case '5':
            case 'F':
            case 'R':
            case 'B':
				wt = 6;
                break;
            default:
                wt = -1;
                break;
		}
        sum += wt*weight[60-pmtlength+sz];
    }
    res[18] = sum/total[pmtlength-1]-1.;
#endif
	
	//TODO: what if the payment string is empty?

	res[19]=un_rate;
	
	res[20]=prep_inc;

	res[21]=principal/100000.0;

    //sea = SEASON%12;
	if (SEASON<4) {
		//res.push_back(0);
		//res.push_back(0);
		//res.push_back(0);
		res[22]=0;
		res[23]=0;
		res[24]=0;
	}
	else if (SEASON<7) {
		//res.push_back(1);
		//res.push_back(0);
		//res.push_back(0);
		res[22]=1;
		res[23]=0;
		res[24]=0;
	}
	else if (SEASON<10){
		//res.push_back(0);
		//res.push_back(1);
		//res.push_back(0);
		res[22]=0;
		res[23]=1;
		res[24]=0;
	}
	else {
		//res.push_back(0);
		//res.push_back(0);
		//res.push_back(1);
		res[22]=0;
		res[23]=0;
		res[24]=1;
	}
}
