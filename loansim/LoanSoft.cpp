#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include "sse_mathfun.h"
#include "ReadData.hpp"
#include "LoanUpdate.hpp"
#include "NewSim.h"
#include "CurveCalc.hpp"
#include "LoanDef.h"
#include <boost/thread.hpp>
#include <boost/mem_fn.hpp>
#include "Assumption.hpp"
using namespace std;

// input parameters structure
enum InputParams {
	FILE_PATH = 1,      //path of dealinfo files
	ASS_FN,				//folder contains assumptions files 
	HPA_FN,				//home price assumption file
	LOAN_FN,			//dealinfo file
	LIBOR_FN,			//LIBOR rate file
	UNEMP_FN,			//unemployment rate file
	MORTRATE_FN,		//mortgage rate file
	TRIAL_NUM,			//number of simulations
	NUM_OF_MONTHS,		//number of months to simulate
	REPORT_TYPE,		//curves types to report
	HAMP_FLG,			//HAMP program flag
	MOD_RATE,			//loan modification rate
	SCE_NM,				//scenario name
	HAMP_BGA,			//HAMP begin period
	HAMP_EDA,			//HAMP end period
	MOD_BGA,			//modification begin period
	MOD_EDA,			//modification end period
	NUM_OF_INPUTS		//number of input parameters
};

//instructions of input parameters string, corresponding to the definition above
static string inputs[NUM_OF_INPUTS] = {
	"[Root Path]", "[Assumption Path]", "[Pre CBSA HPA]", 
	"[Loan Info]", "[Libor]", "[Unemploy]", "[Mort_rate]", 
	"[Number of Trials]", "[Num of Months]", "[Report type]", "[HAMP Percentage]", "[Mod rate]", "[Scenario]",
	"[HAMP Begin Time]", "[HAMP Ending Time]", "[Mod Begin Time]", "[Mod Ending Time]"
};


//define the number of report types
const int numOfReportTypes = 15;

//reports available:
// MDR, MLR, AggMDR, SMM, CDR, CPR, 
// CMM, MOD, LifePre, ThreePre, LifeMod,
// MDeq, Deq3, Deq6, Deq9, DeqF, DeqR
// MSev
// Def, Int, Prin, $Loss, Bal
// Libor
// CumLoss, CumDef


//define the set of available report types
static string reportTypes[numOfReportTypes] = {"MDR",
	"AggMDR", "SMM", "CumLoss", "CumDef", "Bal", 
	"CDR", "CPR", "MSev", "MDeq", "Prin", "Int", "Loss", "Pre", "CF"
};

const string space = " ";
const string comma = ",";


//function to print all the instruction of command line usage
void printInstructions(){
	string s = "LoanSoft.exe ";
	for (int i=0; i<NUM_OF_INPUTS; i++){
		s += inputs[i] + space;
	}

	cout << s << endl << endl;
	cout << "Reports available: ";;
	s.clear();
	for (int i=0; i<numOfReportTypes; i++){
		s += reportTypes[i] + comma + space;
	}
	s += "All";
	cout << s << endl;
}


//functions to verify report type parameter
bool verifyReportType(string type){
	if (type == "All") return true; 

	for (int i=0; i<numOfReportTypes; i++){
		if (type == reportTypes[i]) return true;
	}

	//cout << endl << "Requested type: " << type << " does not match valid reports" << endl;
	return false;
}

int main(int argc, char *argv[]) {
    //clock_t t1, t2;
    //t1 = clock();
    timer t;

	//srand(static_cast<unsigned long>(time(NULL)));  //initialize random seed
	srand(0);

	string fdpath, asspath, cofpath, sevpath, hpapath, loanpath, outpath, liborpath, unemppath, mortpath, scepath;  // scenario path is for scenarios
	string hpapath_n, loanpath_n, liborpath_n, report, unemppath_n, mortpath_n;
    //unsigned int trailnum, hamp_bg, hamp_ed, mod_bg, mod_ed, numOfMonths;
    unsigned int trailnum, numOfMonths, numOfThreads;
    //double modrate, hp_flag;

	//test with constant test files

	//print usage screen
	//cout << "                   Model Version: " << VERSION << endl;

	if (argc < NUM_OF_INPUTS) {
		printInstructions();
		return 1;
	}

	//initialize all assumption files path
	fdpath = string(argv[FILE_PATH]);
	asspath = string(argv[ASS_FN]);
	hpapath_n = string(argv[HPA_FN]);
	loanpath_n = string(argv[LOAN_FN]);
	liborpath_n = string(argv[LIBOR_FN]);
	unemppath_n = string(argv[UNEMP_FN]);
	mortpath_n = string(argv[MORTRATE_FN]);
	trailnum = atoi(argv[TRIAL_NUM]);
	report = string(argv[REPORT_TYPE]);
	numOfMonths = atoi(argv[NUM_OF_MONTHS]);
    //hp_flag = atof(argv[HAMP_FLG]);
    //modrate = atof(argv[MOD_RATE]);
	scepath = string(argv[SCE_NM]);

    if(argc == NUM_OF_INPUTS +1){
        numOfThreads = atoi(argv[NUM_OF_INPUTS]);
    }
    else {
        numOfThreads = boost::thread::hardware_concurrency();
    }

    //hamp_bg = atoi(argv[HAMP_BGA]);
    //hamp_ed = atoi(argv[HAMP_EDA]); 
    //mod_bg = atoi(argv[MOD_BGA]);
    //mod_ed = atoi(argv[MOD_EDA]);

    //if (numOfMonths < 0){
		//cout << "Number of months needs to be greater than 1" << endl;
        //return 1;
        //}

        //if (!verifyReportType(report)) return 1;

	hpapath=asspath;
	hpapath.append(hpapath_n);

	loanpath=fdpath;
	loanpath.append(loanpath_n);

    //cout << "test1" << endl;

	vector<LoanDef> vloan;
	ifstream inFile(loanpath.c_str());
	if (!inFile) {
		cerr<<"Unable to open file: "<< loanpath << endl;
		return 1;
	}
	vloan = read_loan_all(inFile);
    double orig_bal = 0.;
    for(vector<LoanDef>::iterator vvl=vloan.begin(); vvl!=vloan.end(); vvl++)
        orig_bal += vvl->get_BAL();
	
    //cout << "test2" << endl;
	liborpath=asspath;
	liborpath.append(liborpath_n);

	unemppath=asspath;
	unemppath.append(unemppath_n);

	mortpath=asspath;
	mortpath.append(mortpath_n);

    //cofpath=asspath;
    //cofpath.append("coeff.txt");
	
	sevpath=asspath;
	sevpath.append("sevcof.txt");


	//reading of all assumption files and store them into proper structures
    Assumption assump(hpapath, liborpath, unemppath, mortpath, numOfMonths, trailnum);

    //map<string, vector< vector<double> > > coeff;
    //coeff = map_double_vec(cofpath);

	unordered_map<string, vector<double> > sev_coeff = getdata(sevpath);

	NextStepCalcML nsc;

    cout << "All assumptions read in" << endl;
    vector<double> v1(numOfMonths, 0.);
    vector<vector<double> > v2(7, v1);
    vector<vector<vector<double> > >res(trailnum, v2);

    cout << "numOfThreads is : " << numOfThreads << endl;
    vector<boost::thread> threads(numOfThreads);
    //boost::thread_group g;
    unsigned int perTrail = trailnum/numOfThreads;
    unsigned int run_bg, run_ed = 0;
    //cout << "perTrail is : " << perTrail << endl;

    if(numOfThreads > 1){
        for(unsigned int i = 0; i!=numOfThreads-1; ++i){ 
            run_bg = i*perTrail;
            run_ed = (i+1)*perTrail;
            cout << "thread " << i << ":" << run_bg << "|" << run_ed << endl;
        //Deal_Sim(vloan, assump, sev_coeff, numOfMonths, nsc, res[run], run);
            threads[i] = boost::thread(Deal_Sim, boost::ref(vloan), boost::ref(assump), boost::ref(sev_coeff), numOfMonths, nsc, boost::ref(res), run_bg, run_ed);
        }
        cout << "thread " << numOfThreads-1 << ":" << run_ed << "|" << trailnum << endl;
        run_bg = run_ed+1;
        run_ed = trailnum;
        threads[numOfThreads-1] = boost::thread(Deal_Sim, boost::ref(vloan), boost::ref(assump), boost::ref(sev_coeff), numOfMonths, boost::ref(nsc), boost::ref(res), run_bg, run_ed);
        for_each(threads.begin(), threads.end(), boost::mem_fn(&boost::thread::join));
    }

    else{
        Deal_Sim(vloan, assump, sev_coeff, numOfMonths, nsc, res, 0, trailnum);
    }

    cout<<"Now Produce Curves"<<'\n';
	string root = fdpath.append(loanpath_n.substr(0, loanpath_n.length()-4));

	for (int i=0; i<numOfReportTypes; i++){
		if (report == "All" || report == reportTypes[i]){
			string file = root + "-" + reportTypes[i] + "-" + scepath + ".txt";
			if(reportTypes[i]=="MSev") {  //|| reportTypes[i]=="MDeq"
				write_doublevec_b(file, get_curve(res, reportTypes[i], trailnum, numOfMonths, orig_bal));
			}
			
			unsigned int pre = (reportTypes[i]=="Bal"|| reportTypes[i]=="Prin" || reportTypes[i]=="Int" || reportTypes[i]=="Loss" || reportTypes[i]=="Pre" || reportTypes[i]=="CF")?2:3;
            write_doublevec(file, get_curve(res, reportTypes[i], trailnum, numOfMonths, orig_bal), pre);
		}
	}
	return 0;
}
