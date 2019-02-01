#include "CurveCalc.hpp"
#include <cmath>

using namespace std;

//get lifetime prepayment rate based on given length of CPR curve
double getlife(vector<double> vec) {
    vector<double>::iterator iter1;
    double res = 1.0;
    int ct = 1;
    for(iter1=vec.begin(); iter1!=vec.end(); iter1++) {
        res = res*(1-(*iter1));
        ct++;
    }
    res = 1.0 - res;
    res = 1-pow((1-res),1.0/ct);
    return res;
}


//convert the resulting big array into proper curves based on the set of curves types defined
//return the vector of all curves required, each curve is a vector of double
vector<vector<double> > get_curve(vector<vector<vector<double> > > &data, string choice, unsigned int trail, unsigned int month, double orig_bal_vv) {
    vector<vector<double> > res, resx, resy;
    vector<double> midres, midresx;
    double mid=0;
    double midmid=0;

    //vector<vector<double> > deal_int_pay = data[0];			//interest payment
    //vector<vector<double> > deal_prin_pay = data[1];		//principal payment
    //vector<vector<double> > deal_re_bal = data[2];			//remaining balance	
    //vector<vector<double> > deal_pre_pay = data[3];			//prepayment payment
    //vector<vector<double> > deal_def_pay = data[4];			//recovery amount
    //vector<vector<double> > deal_deq_m = data[5];			//delinquent
    //vector<vector<double> > deal_def_amt=data[6];			//default amount

    //set dollar amount to zero if it is less than some threshold
    for (unsigned int i=0; i<trail; i++){
        for (unsigned int j=0; j<month; j++){
            if (data[i][2][j] <20.0) data[i][2][j]=0.0;		
            if (data[i][5][j] <20.0) data[i][5][j]=0.0;
            if (data[i][6][j] <5.0) data[i][6][j]=0.0;
            if (data[i][4][j] <5.0) data[i][4][j]=0.0;
            if (data[i][3][j] <5.0) data[i][3][j]=0.0;
        }

    }

    //conditional default rate
    if (choice == "CDR"){
        for (unsigned int i=0; i<trail; i++){
            midres.push_back(1-pow(1-data[i][6][0]/orig_bal_vv, 12.));
            for (unsigned int j=1; j<month; j++){
                if (data[i][2][j-1] > 0.1)
                    mid = 1-pow(1-data[i][6][j]/data[i][2][j-1], 12.);
                else
                    mid = 0.;
                midres.push_back(mid);
            }
            res.push_back(midres);
            midres.clear();
        }
    }

    else if(choice=="MDR") {
        for(unsigned int i=0; i<trail; i++) {
            midres.push_back(data[i][6][0]/orig_bal_vv);
                for(unsigned int j=1; j<month; j++) {
                    if (data[i][2][j-1]>0.1) {
                        mid=data[i][6][j]/data[i][2][j-1];
                    }
                    else {
                        mid=0;
                    }
                    midres.push_back(mid);
                }
                res.push_back(midres);
                midres.clear();
        }
    }

    //aggregated monthly default rate
    else if(choice=="AggMDR") {
        for(unsigned int i=0; i<trail; i++) {
                for(unsigned int j=0; j<month; j++) {
                    if(orig_bal_vv>0) {
                        mid=data[i][6][j]/orig_bal_vv;
                        midres.push_back(mid);
                    }
                    else midres.push_back(0);
                }
                res.push_back(midres);
                midres.clear();
        }
    }

    //single monthly mortality 
    else if(choice=="SMM") {
        for(unsigned int i=0; i<trail; i++) {
            midres.push_back(data[i][3][0]/orig_bal_vv);
                for(unsigned int j=1; j<month; j++) {
                    if (data[i][2][j-1]>0.1) {
                        mid=data[i][3][j]/data[i][2][j-1];
                    }
                    else {
                        mid=0.;
                    }
                    midres.push_back(mid);
                }
                res.push_back(midres);
                midres.clear();
            }
    }

    //conditional prepayment rate
    else if (choice == "CPR"){
        for (unsigned int i=0; i<trail; i++){
            midres.push_back(1-pow(1-data[i][3][0]/orig_bal_vv, 12.));
            for (unsigned int j=1; j<month; j++){
                if (data[i][2][j-1] > 0.1)
                    mid = 1-pow(1-data[i][3][j]/data[i][2][j-1], 12.);
                else
                    mid = 0.;
                midres.push_back(mid);
            }
            res.push_back(midres);
            midres.clear();
        }
    }

    //lifetime prepayment rate
    /*
    else if(choice == "LifePre") {
        for(unsigned int i=0; i<trail; i++) {
            midres.push_back(data[i][3][0]/orig_bal_vv);
                for(unsigned int j=1; j<month; j++) {
                    if (data[i][2][j-1]>0.1) {
                        mid=data[i][3][j]/data[i][2][j-1];
                    }
                    else {
                        mid=0;
                    }
                    midres.push_back(mid);
                }
                midresx.push_back(getlife(midres));
                res.push_back(midresx);
                midres.clear();
                midresx.clear();
            }
    }
    */
    
    //monthly delinquent rate
    else if(choice=="MDeq") {
        for(unsigned int i=0; i<trail; i++) {
            midres.push_back(data[i][5][0]/orig_bal_vv);
                for(unsigned int j=1; j<month; j++) {
                    if (data[i][2][j-1]>1.0) {
                        mid=data[i][5][j]/data[i][2][j-1];
                    }
                    else {
                        mid=0;
                    }
                    midres.push_back(mid);
                }
                res.push_back(midres);
                midres.clear();
        }
    }

    //Monthly Severity
    else if(choice=="MSev") {
        for(unsigned int i=0; i<trail; i++) {
                for(unsigned int j=0; j<month; j++) {
                    if (data[i][6][j]>0.1) {
                        mid=1-data[i][4][j]/data[i][6][j];
                    }
                    else {
                        mid=0;
                    }

                    if(mid<0) mid=0;

                    midres.push_back(mid);
                }
                res.push_back(midres);
                midres.clear();
        }   
    }

    //default amount
    else if (choice=="Def") {
        for(unsigned int i=0; i<trail; i++) {
            res.push_back(data[i][6]);
        }
    }

    //prepay amount
    else if (choice=="Pre") {
        for(unsigned int i=0; i<trail; i++) {
            res.push_back(data[i][3]);
        }
    }
        
    //interest payment
    else if (choice=="Int") {
        for(unsigned int i=0; i<trail; i++) {
            res.push_back(data[i][0]);
        }
    }

    //principal payment
    else if (choice=="Prin") {
        for(unsigned int i=0; i<trail; i++) {
            res.push_back(data[i][1]);
        }
    }

    else if (choice=="CF") {
        for(unsigned int i=0; i<trail; i++) {
            for(unsigned int j=0; j<month; j++){
                midres.push_back(data[i][1][j]+data[i][0][j]);
            }
            res.push_back(midres);
            midres.clear();
        }   
    }

    //loss amount in $
    else if (choice=="Loss") {

        for(unsigned int i=0; i<trail; i++) {
                for(unsigned int j=0; j<month; j++) {
                    mid=data[i][6][j]-data[i][4][j];
                    midres.push_back(mid);
                }
                res.push_back(midres);
                midres.clear();
        }
        
    }

    //remaining balance
    else if (choice=="Bal") {
        for(unsigned int i=0; i<trail; i++) {
            res.push_back(data[i][2]);
        }
    }

    //cumulative loss rate
    else if (choice=="CumLoss") {
        for(unsigned int i=0; i<trail; i++) {
                midmid=0;
                for(unsigned int j=0; j<month; j++) {
                    mid=data[i][6][j]-data[i][4][j];
                    midmid += mid;
                }
                midres.push_back(midmid/orig_bal_vv);
                res.push_back(midres);
                midres.clear();
        }
    }

    //cumulative default rate
    else if (choice=="CumDef") {
        for(unsigned int i=0; i<trail; i++) {
                midmid=0;
                for(unsigned int j=0; j<month; j++) {
                    mid=data[i][6][j];
                    midmid += mid;
                }
                midres.push_back(midmid/orig_bal_vv);
                res.push_back(midres);
                midres.clear();
        }
    }

    return res;
    //res.clear();

    }
