#include "LoanUpdate.hpp"
#include "NRand.h"
#include "LoanDef.h"
#include "NextStepCalc.h"
#include "Amort.h"
#include "SevCalc.h"
#include "ReadData.hpp"

using namespace std;

//Updates the delinquency score by using different weight on different delinquency statuses
//return updated delinquency score
double pct_deno(int age, char status, double pct) {
    double deno=0.0;
    double denox=0.0;
    double wt;
    for(int i=0; i<age && i<60; i++) {
        deno += exp_ps((20.-i)/40.);
    }
    if (age<61) {
        denox = deno*exp_ps(-1/40.) + exp_ps(0.5);
    }
    else {
        denox=deno;
    }

    if (status =='C') wt=1;
    else if(status =='3') wt=2;
    else if(status=='6') wt=4;
    else wt=6;

    return ((pct+1.)*deno*exp_ps(-1/40.)+wt*exp_ps(0.5))/denox - 1;
}

//calculates the target interest rate to bring DTI down to 31% for HAMP by using the Newton method
//and returns interest rate calculated
double solve_int(LoanDef &loan) {
    vector<double> amor_res;
    amort am;
    double rate_l, rate_r, rate_m;
    rate_l = 0.02; 
    rate_r = loan.get_INT_RATE()/100;

    double endbal;
    double monpay;

    monpay = loan.get_prin_pay()+loan.get_int_pay();
    endbal = monpay/loan.get_DTI()*100;

    double sol_dti;

    //Newton method
    for (int i=0; i<10; i++) {
        rate_m = (rate_l + rate_r)/2.0;
        amor_res = am.amort_FRM(loan.get_AGE(), loan.get_BAL(), loan.get_TERM(), rate_m, 0, loan.get_amor().amort_term);
        monpay = amor_res[0]+amor_res[1];
        sol_dti = monpay/endbal;
        if (sol_dti-0.31>0.01) {
            rate_r = rate_m;
        }
        else {
            rate_l = rate_m;
        }
    }

    return rate_m;
}

//the corresponding loan status update function with modification
void loanmod_next_status(LoanDef &loan, double libor, double hpa, double unemploy, double mort_rate,
                 double orig_bal, double orig_ltv, double orig_nltv,
                 unordered_map<string, vector<double> > &sev_coeff, 
                 NextStepCalcML &nsc){

    //transition of 0->0, just assign zero
    if(loan.get_STAT()=='0') {
        loan.add_AGE();
        return;
        //loan.change_int_pay(0.0);
        //loan.change_prin_pay(0.0);
        //loan.change_LTV(0.0);
        //loan.change_def_amt(0.0);
        //loan.change_init(0.0);
        //loan.change_BAL(0.0);
    }

    //shouldn't be here -- paid off
    if(loan.get_BAL()<0.01) {
        loan.change_LTV(0);
        loan.change_NLTV(0);
        loan.change_BAL(0.0);
        loan.change_prin_pay(0);
        loan.change_int_pay(0);
        loan.change_STAT('0');
        loan.change_def_amt(0);
        loan.change_init(0);
        return;
    }

    /*
    if(loan.get_AGE()==loan.get_TERM()) {
        // if it expires
        loan.change_STAT('0');
        loan.add_AGE();
        loan.change_int_pay(0.0);
        loan.change_prin_pay(0.0);
        loan.change_LTV(0.0);
        loan.change_def_amt(0);
        return;
    }       
    */

    bool check_nan = false;
    bool check_inf = false;
    // this is used to update the status of the loan in the next stage
    // first increase age by one
    SevCalc sev_calc;

    //loan term ends
    if(loan.get_AGE()>loan.get_TERM()) {
        loan.change_LTV(0);
        loan.change_NLTV(0);
        loan.change_prin_pay(loan.get_BAL()*(1-sev_calc.sev(loan.get_STAT(), loan, sev_coeff))); 
        loan.change_int_pay(0);
        loan.change_STAT('0');
        loan.change_def_amt(loan.get_BAL());
        loan.change_BAL(0.0);
        loan.change_STAT('0');
        loan.change_init(0);
        return;
    }

    //updating loan status based on simulated next status
    else {
        vector<double> amor_res;
        amort am;
        double ltv_mid, ltv_mid2, last_ltv, ch_ltv;
        double bal_midx;
        int deq_m;

        char stg_mid=loan.get_STAT();
        deq_m = loan.get_deq_m();
        last_ltv = loan.get_LTV();

        //loan is still alive
            //update age and update next status
        loan.change_STAT(nsc.next_stg_c(loan)); 
        //TODO: should add age after amortization or before?
        loan.add_AGE();

        if(loan.get_amor().neg_limit>0 && loan.get_STAT() != 'C') {
            loan.get_amor().change_neg_limit(0);
            loan.reset_pmt_shock();
        }

        if(loan.get_amor().neg_limit>0) {
            amor_res = am.amort_NEG_X(loan.get_BAL(), loan.get_INT_RATE()/100.0, loan.get_TERM(), loan.get_AGE()-loan.get_deq2(), loan.get_F_RATE(), libor, loan.get_amor() );
            int i = 0;
            for(vector<double>::const_iterator it=amor_res.begin(); it!=amor_res.end(); ++it, ++i){
                if(check_inf && !IsFiniteNumber(*it)) {
                    cout << "Check NEG part " << i << ", " << *it << endl;
                }
            }
        }

        else if(loan.get_PROD_TYPE().at(0)=='1'||loan.get_PROD_TYPE().at(0)=='5') {
            amor_res = am.amort_FRM(loan.get_AGE()-loan.get_deq2(), loan.get_BAL(), loan.get_TERM(), loan.get_INT_RATE()/100.0, loan.get_amor().io, loan.get_amor().amort_term);
            int i = 0;
            for(vector<double>::const_iterator it=amor_res.begin(); it!=amor_res.end(); ++it, ++i){
                if(check_inf && !IsFiniteNumber(*it)) {
                    cout << "Check FRM part " << i << ", " << *it << endl;
                }
            }
        }

        else {
            amor_res = am.amort_ARM_X(loan.get_BAL(), loan.get_INT_RATE()/100.0, loan.get_TERM(), loan.get_AGE()-loan.get_deq2(), loan.get_F_RATE(), libor, loan.get_amor() );
            int i = 0;
            for(vector<double>::const_iterator it=amor_res.begin(); it!=amor_res.end(); ++it, ++i){
                if(check_inf && !IsFiniteNumber(*it)) {
                    cout << "Check ARM part " << i << ", " << *it << endl;
                    cout << loan.get_NAME() << ": Current balance is " << loan.get_BAL() << endl;
                }
            }
        }

        if(deq_m>0) {
            loan.change_int_acc(deq_m*amor_res[0]);
            loan.change_prin_acc(deq_m*amor_res[1]);
            loan.change_deq_m(0);
        }

        if(deq_m<0) {
            loan.change_deq_m(++deq_m);
            loan.change_STAT('C');
        }


        // update interest rate
        loan.change_INT_RATE(amor_res[3]*100);

        // update loan balance and different payment, accrual account
        if(loan.get_STAT()=='C') {
            // if loan changes to C
            loan.change_int_pay(amor_res[0]+loan.get_int_acc());
            loan.change_prin_pay(amor_res[1]+loan.get_prin_acc());
            /* QUESTION */
            if(amor_res[2]<0.01) { amor_res[2] = 0; }

            //CHECK
            loan.change_BAL(loan.get_BAL()-loan.get_prin_pay());
            if(check_nan && isNA(loan.get_BAL())) cout << "C" << endl;
            if(loan.get_BAL()<0.001) loan.change_BAL(0.0);
            /* QUESTION ENDS */
            loan.change_int_acc(0);
            loan.change_prin_acc(0);
            loan.change_def_amt(0);
            loan.change_deq2(0);
            //loan.change_prep_inc( ((loan.get_INT_RATE()-mort_rate)*loan.get_BAL()*1.2-0.12*loan.get_INT_RATE()*loan.get_BAL())/100000. );
            //update later

        }

        //prepay situation
        else if(loan.get_STAT()=='0' && stg_mid=='C') {
            // if changes from C->0, means prepay
            bal_midx=loan.get_BAL();
            if(bal_midx<0.01) {
                loan.change_int_pay(0);
            }
            else {
                loan.change_int_pay(loan.get_INT_RATE()/1200*loan.get_BAL());
            }
            loan.change_prin_pay(loan.get_BAL());
            loan.change_BAL(0.0);
            loan.change_int_acc(0);
            loan.change_prin_acc(0);
            loan.change_def_amt(0);
            loan.change_deq2(0);

        }

        //loan defaults
        else if(loan.get_STAT()=='0' && stg_mid!='C' && stg_mid!='3' && stg_mid!='6' && stg_mid!='0') {
            // if it defaults, i.e. from 9,F,R, apply severity
            loan.change_int_pay(0);
            loan.change_prin_pay(loan.get_BAL()*(1-sev_calc.sev(stg_mid, loan, sev_coeff)));  // only the severity part is lost
            //double loss_mid;
            //if(loss_mid<0) loss_mid=0;
            //loan.change_prin_pay(loan.get_BAL()-loss_mid);

            loan.change_def_amt((loan.get_BAL()));
            loan.change_BAL(0.0);
            loan.change_int_acc(0.0);
            loan.change_prin_acc(0.0);
            loan.change_init(0.0);
            loan.change_deq2(0);

        }

        //transition to foreclosure or REO
        else if (loan.get_STAT()== 'F' || loan.get_STAT()=='R'){
            loan.change_int_pay(0);
            loan.change_prin_pay(0);
            loan.change_int_acc(loan.get_int_acc()+amor_res[0]);  //accrual interest account
            loan.change_prin_acc(loan.get_prin_acc()+amor_res[1]);  // accrual principal account
            loan.change_def_amt(0);
            loan.change_deq2(loan.get_deq2()+1);

        }

        //transition of 3->3
        else if(loan.get_STAT()=='3' && stg_mid=='3') {
            //loan.change_int_pay(loan.get_int_acc());
            //loan.change_prin_pay(loan.get_prin_acc());
            // NEW
            //loan.change_BAL(loan.get_BAL()-loan.get_prin_acc());
            loan.change_int_pay(amor_res[0]);
            loan.change_prin_pay(amor_res[1]);
            //CHECK
            loan.change_BAL(amor_res[2]);
            if(check_nan && isNA(loan.get_BAL())) cout << "3->3" << endl;
            loan.change_int_acc(amor_res[0]);
            loan.change_prin_acc(amor_res[1]);
            loan.change_def_amt(0);
            //loan.change_deq2();
        }

        //transition of C->3
        else if(loan.get_STAT()=='3' && stg_mid=='C') {
            loan.change_int_pay(0);
            loan.change_prin_pay(0);
            loan.change_int_acc(amor_res[0]);
            loan.change_prin_acc(amor_res[1]);
            loan.change_def_amt(0);
            loan.change_deq2(1);
        }

        //transition of 6->3
        else if(loan.get_STAT()=='3' && stg_mid=='6') {
            loan.change_int_pay(2*amor_res[0]);
            loan.change_prin_pay(2*amor_res[1]);
            //NEW
            loan.change_BAL(loan.get_BAL()-2*amor_res[1]);
            if(check_nan && isNA(loan.get_BAL())) cout << "3->6" << endl;
            loan.change_int_acc(amor_res[0]);
            loan.change_prin_acc(amor_res[1]);
            
            loan.change_def_amt(0);
            loan.change_deq2(1);
        }

        //transition of 3->6
        else if(loan.get_STAT()=='6' && stg_mid=='3') {
            loan.change_int_pay(0);
            loan.change_prin_pay(0);
            loan.change_int_acc(amor_res[0]+loan.get_int_acc());
            loan.change_prin_acc(amor_res[1]+loan.get_prin_acc());
            loan.change_def_amt(0);
            loan.change_deq2(2);
        }

        //transition of 6->6
        else if(loan.get_STAT()=='6' && stg_mid=='6') {
            loan.change_int_pay(amor_res[0]);
            loan.change_prin_pay(amor_res[1]);
            loan.change_BAL(amor_res[2]);
            if(check_nan && isNA(loan.get_BAL())) cout << "6->6" << endl;
            loan.change_int_acc(2*amor_res[0]);
            loan.change_prin_acc(2*amor_res[1]);
            loan.change_def_amt(0);
            //loan.change_deq2();
        }

        //transition of 9->6
        else if(loan.get_STAT()=='6' && stg_mid=='9') {
            loan.change_int_pay(2*amor_res[0]);
            loan.change_prin_pay(2*amor_res[1]);
            //NEW
            loan.change_BAL(loan.get_BAL()-2*amor_res[1]);
            if(check_nan && isNA(loan.get_BAL())) cout << "6->9" << endl;
            loan.change_int_acc(2*amor_res[0]);
            loan.change_prin_acc(2*amor_res[1]);
            loan.change_def_amt(0);
            loan.change_deq2(2);
        }

        //transition of 6->9
        else if(loan.get_STAT()=='9' && stg_mid=='6') {
            loan.change_int_pay(0);
            loan.change_prin_pay(0);
            loan.change_int_acc(loan.get_int_acc()+amor_res[0]);
            loan.change_prin_acc(loan.get_prin_acc()+amor_res[1]);
            loan.change_def_amt(0);
            loan.change_deq2(3);
        }

        //transition of 9->9
        else if(loan.get_STAT()=='9' && stg_mid=='9') {
            loan.change_int_pay(amor_res[0]);
            loan.change_prin_pay(amor_res[1]);
            //NEW
            loan.change_BAL(amor_res[2]);
            if(check_nan && isNA(loan.get_BAL())) cout << "9->9" << endl;
            loan.change_int_acc(3*amor_res[0]);
            loan.change_prin_acc(3*amor_res[1]);
            loan.change_def_amt(0);
            //loan.change_deq2();
        }

        //transition of F->9
        else if(loan.get_STAT()=='9' && stg_mid=='F') {
            loan.change_int_pay(loan.get_int_acc()-2*amor_res[0]);
            loan.change_prin_pay(loan.get_prin_acc()-2*amor_res[1]);
            // NEW
            loan.change_BAL(loan.get_BAL()-loan.get_prin_pay());
            if(check_nan && isNA(loan.get_BAL())){ 
                cout << "F->9" << endl;
                if(check_inf && IsFiniteNumber(loan.get_prin_acc())) cout << "Check get prin acc" << endl;
                if(check_inf && IsFiniteNumber(loan.get_prin_pay())) cout << "Check get prin pay" << endl;
            }
            loan.change_int_acc(3*amor_res[0]);
            loan.change_prin_acc(3*amor_res[1]);
            loan.change_def_amt(0);
        }		

        //update delinquent history
#ifdef NO_DEQHIST_UPDATE
        loan.change_deq_hist(pct_deno(loan.get_AGE(), loan.get_STAT(), loan.get_deq_hist()));
#else
        //update payment history string
        if(loan.pmtHistory.length()<60) loan.pmtHistory.push_back(loan.STAT);
        else loan.pmtHistory.erase(0, 1).push_back(loan.STAT);
#endif

        //update combined loan-to-value ratio by using new HPA index and new loan balance
        ltv_mid = orig_ltv*loan.get_BAL()/orig_bal/hpa;
        if(ltv_mid>250||isNA(ltv_mid)) {
            ltv_mid=loan.get_LTV();
        }
        loan.change_LTV(ltv_mid);

        //update combined loan-to-value ratio by using new HPA index and new loan balance
        ltv_mid2 = orig_nltv*loan.get_BAL()/orig_bal/hpa;
        if(ltv_mid2>250||isNA(ltv_mid2)) {
            ltv_mid2=loan.get_NLTV();
        }
        loan.change_NLTV(ltv_mid2);

        //update difference of loan-to-value ratio
        ch_ltv = ltv_mid - last_ltv;
        loan.change_dltv(ch_ltv);

        //update payment shock
        if (loan.get_pmt_shock()!=600) {
            loan.change_pmt_shock();
        }

        //update prepay incentive

        if (loan.get_STAT()=='C') {
            // all profit minus the cost
            loan.change_prep_inc(((loan.get_INT_RATE()-mort_rate)*loan.get_BAL()*1.2
                -0.12*loan.get_INT_RATE()*loan.get_BAL())/100000.);
        }

        //update unemployment rate
        loan.change_un_rate(unemploy);

        //update season
        loan.change_SEASON();

        //update remaining REO time
        if (loan.get_STAT()=='R') {
            loan.change_reo_rem();
        }

        loan.update_vec();
    }
}
