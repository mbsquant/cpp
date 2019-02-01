#include <vector>
#include "LoanDef.h"
#include "NewSim.h"
#include "LoanUpdate.hpp"
using namespace std;

void Deal_Sim(vector<LoanDef> &vl, 
        Assumption &assump,
        unordered_map<string, vector<double> > &sev_coeff, 
        unsigned int numOfMonths, NextStepCalcML &nsc, vector<vector<vector<double> > > &res, unsigned int run_bg, unsigned int run_ed){
    for(unsigned int run = run_bg; run!=run_ed; ++run){
        //cout << "run " << run << endl;
        //make a copy of loan vector
        vector<LoanDef> vloan(vl);
        //vector<double> hpa, unemp;
        char stat_mid;
        double orig_ltv, orig_bal, orig_nltv;

        vector<LoanDef>::iterator vvl, vvl_end = vloan.end();
        for(vvl=vloan.begin(); vvl!=vvl_end; ++vvl){
            //cout << "running loan" << ++loancnt << endl;
            /*
            //TODO: change the query, shouldn't be anything else
            if(assump.hpa[run].find(vvl->get_CBSA())==assump.hpa[run].end()) {
                hpa = assump.hpa[run]["00000"];
            }
            else {
                hpa = assump.hpa[run][vvl->get_CBSA()];
            }	
            
            //TODO: change the query, shouldn't be anything else
            if(assump.unemp.find(vvl->get_LOC())==assump.unemp.end()) {
                unemp = assump.unemp["AA"];
            }
            else {
                unemp = assump.unemp[vvl->get_LOC()];
            }
            */

            orig_ltv=vvl->get_LTV();
            orig_bal=vvl->get_BAL();
            orig_nltv=vvl->get_NLTV();
            //cout << "original balance is " << orig_bal << endl;

            // restore the necessary information initially
            for(unsigned int i=0; i<numOfMonths; ++i) {
                //double int_pay, prin_pay, re_bal, pre_pay, def_pay, delinq_6, def_amt=0.;
                stat_mid=vvl->get_STAT();

                if(i==0 && (vvl->get_STAT()=='R' || vvl->get_STAT()=='F')) {
                    vvl->change_reo_time(vvl->get_reo_rem());
                    vvl->set_reo_rem();
                    vvl->change_reo_time(vvl->get_reo_rem()+vvl->get_reo_time());
                }
               
                //TODO: remove these debug print
                //cout << assump.libor[run][i] << "\n";
                //cout << vvl->get_CBSA() << "|";
                //cout << assump.hpa[run][string("40140")][i+1] << "\n";
                //cout << assump.hpa[run][vvl->get_CBSA()][i+1] << "\n";
                //cout << assump.unemp[vvl->get_LOC()][i] << "\n"; 
                //cout << assump.mort[run][i] << "\n";
                //cout << orig_bal << "\n";
                //cout << orig_ltv << "\n";
                //cout << orig_nltv << "\n";

                //call the next status function
                loanmod_next_status(*vvl, 
                        assump.libor[run][i], 
                        assump.hpa[run][vvl->CBSA][i+1], 
                        assump.unemp[vvl->LOC][i], 
                        assump.mort[run][i], 
                        orig_bal, orig_ltv, orig_nltv, sev_coeff, nsc);
               
                if(vvl->get_STAT() == '6' && stat_mid == '3' && !vvl->get_ltv_flag()){
                    vvl->change_LTV(vvl->get_LTV()+30.0);
                    vvl->change_NLTV(vvl->get_NLTV()+30.0);
                    vvl->set_ltv_flag(true);
                }
                
                if(vvl->get_STAT()=='R' && stat_mid!='R') {
                    // set REO timeline if just a transfer to REO
                    vvl->set_reo_rem();
                    //vvl->change_reo_time(vvl->get_reo_rem());
                }
                
                //prepay
                if(stat_mid=='C' && vvl->get_STAT()=='0' && (abs(static_cast<int>(vvl->get_AGE()-vvl->get_TERM()))>2)) {
                    res[run][PRE_PAY][i]+=vvl->get_prin_pay();
                }
                // loan modification prepay
                if(stat_mid=='F' && vvl->get_STAT()=='0') {
                    //cout<<"Adding";
                    res[run][PRE_PAY][i]+=vvl->get_init();
                }
                //default
                if(stat_mid!='C' && stat_mid!='3' && stat_mid!='6' && stat_mid!='0' && vvl->get_STAT()=='0') {
                    res[run][DEF_PAY][i] += vvl->get_prin_pay();
                    res[run][DEF_AMT][i] += vvl->get_def_amt();
                }

                //delinquent
                if(vvl->get_STAT()=='6' || vvl->get_STAT()=='9' || vvl->get_STAT()=='F' || vvl->get_STAT()=='R') {
                    res[run][DELINQ_M][i] += vvl->get_BAL();
                }

                res[run][INT_PAY][i]+=vvl->get_int_pay();
                res[run][PRIN_PAY][i]+=vvl->get_prin_pay();
                res[run][RE_BAL][i]+=vvl->get_BAL();	
            } 
        }
    }
}
