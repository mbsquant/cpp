#ifndef ASSUMPTION_HPP
#define ASSUMPTION_HPP

#include <vector>
#include <unordered_map>
#include <string>
#include "LoanDef.h"
#include "ReadData.hpp"

#include "NextStepCalc.h"

using namespace std;

class Assumption{
    friend void Deal_Sim(vector<LoanDef> &vl, Assumption& assump, unordered_map<string, vector<double> > &sev_coeff, unsigned int numOfMonths, NextStepCalcML &nsc, vector<vector<vector<double> > > &res, unsigned int run_bg, unsigned int run_ed);
    private:
        vector<unordered_map<string, vector<double> > > hpa;
        vector<vector<double> > libor;
        unordered_map<string, vector<double> > unemp;
        vector<vector<double> > mort;
    public:
        Assumption(string hpapath, string liborpath, string unemppath, string mortpath, unsigned int numOfMonths, unsigned int trailnum);
};

#endif
