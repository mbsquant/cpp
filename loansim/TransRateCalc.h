#ifndef TRANSRATECALC_H
#define TRANSRATECALC_H
#include <vector>
#include <cmath>
#include <numeric>
#include "sse_mathfun.h"
#include "NextStepCalc.h"

using namespace std;

//TODO: fix this with namespace
typedef double cof1[28];
typedef double cof2[2][28];
typedef double cof4[4][28];

class TransRateCalc {
public:
    const static unsigned int num = 27;
	inline double vecmult(vector<double> &vec1, const cof1 vec2) {
        double res = vec2[0];
        for(unsigned int i=0; i<num; ++i) {
            res += vec1[i]*vec2[i+1];
        }
        return res;
        //return inner_product(vec1.begin(), vec1.begin()+num, vec2.begin()+1, 0.);
	}

	//the probabilities from regression with five possible outcomes
	//return a vector of five probabilities
	vector<double> calc_four(vector<double> &v, const cof4 coeff) {
		double mu1, mu2, mu3, mu4;
		double sum;
		vector<double> res(5,0.);

		mu1 = exp_ps(vecmult(v, coeff[0]));
		mu2 = exp_ps(vecmult(v, coeff[1]));
		mu3 = exp_ps(vecmult(v, coeff[2]));
		mu4 = exp_ps(vecmult(v, coeff[3]));
		sum = mu1 + mu2 + mu3 + mu4 + 1.0;
		res[0]=(mu1/sum);
		res[1]=(mu2/sum);
		res[2]=(mu3/sum);
		res[3]=(mu4/sum);
		res[4]=1.-res[0]-res[1]-res[2]-res[3];
		return res;
	}

	//the probabilities from regression with three possible outcomes
	//return a vector of three probabilities
	vector<double> calc_two(vector<double> &v, const cof2 coeff) {
		double mu1, mu2;
		double sum;
		vector<double> res(3, 0.);
		
		mu1 = exp_ps(vecmult(v, coeff[0]));
		mu2 = exp_ps(vecmult(v, coeff[1]));
		sum = mu1 + mu2 + 1;
		res[0]=(mu1/sum);
		res[1]=(mu2/sum);
		res[2]=1.-res[0]-res[1];
		return res;
	}

	//the probabilities from regression with two possible outcomes
	//return a vector of two probabilities
	vector<double> calc_one(vector<double> &v, const cof1 coeff) {
		double mu1;
		double sum;
		vector<double> res(2, 0.);

		mu1 = exp_ps(vecmult(v, coeff));
		sum = mu1+ 1;
		
		res[0]=(mu1/sum);
		res[1]=1.0-res[0];

		return res;
	}

	//calculate transition from NP, PP to C3, CC,
	//record the corresponding probability of PP, C3, CC
	//return a vector of three probabilities
	vector<double> two_vec(vector<double> &vec1, vector<double> &vec2) {
		//vec1 is NP, PP, vec2 is C3, CC
		// result is PP, C3, CC
		vector<double> res(3, 0.);
        /* /
        double mod = 0.0;
		res[0]=(vec1[1]+mod);
		res[1]=((vec1[0]-mod)*vec2[0]);
		res[2]=((vec1[0]-mod)*vec2[1]);
        */
        //double mod = 0.0;
		res[0]=vec1[1];
		res[1]=vec1[0]*vec2[0];
		res[2]=1.-res[0]-res[1];
		return res;
	}
};

#endif
