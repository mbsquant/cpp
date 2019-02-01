#ifndef NEXTSTEPCALC_H
#define NEXTSTEPCALC_H


#include "LoanDef.h"
#include "TransRateCalc.h"
using namespace std;

//class to set all the transition coefficients for the next transition
class NextStepCalcML {
public:
	char next_stg_c(LoanDef &loan);
    typedef double cof1[28];
    typedef double cof2[2][28];
    typedef double cof4[4][28];
private:
    const static cof1 P_L1_NSK_NI_NP,  P_L1_NSK_NI_P,  P_L1_SK_NI_P,  P_L1_SK_I_P,  P_L1_NSK_I_P,  P_L1_NSK_I_NP,  P_L1_SK_I_NP,  P_L1_SK_NI_NP,  
		P_L2_NSK_NI_P,  P_L2_NSK_NI_NP,  P_L2_NSK_I_NP,  P_L2_NSK_I_P,  P_L2_SK_I_P,  P_L2_SK_I_NP,  P_L2_SK_NI_NP,  P_L2_SK_NI_P,  
		L1_SK_NP,  L2_SK_NP,  L1_SK_P,  L2_SK_P,  L1_NSK_NP,  L2_NSK_NP,  L1_NSK_P,  L2_NSK_P, P_2YR, P_3YR, P_4YR, P_5YR, P_6YR,
		NP_2YR, NP_3YR, NP_4YR, NP_5YR, NP_6YR, L2_P_2YR, L2_P_3YR, L2_P_4YR, L2_P_5YR, L2_P_6YR, L2_NP_2YR, L2_NP_3YR, L2_NP_4YR, L2_NP_5YR, L2_NP_6YR;  
		
    const static cof2 T3_L1_2YR, T3_L1_3YR, T3_L1_4YR, T3_L1_5YR, T3_L1_6YR, T3_L2_2YR, T3_L2_3YR, T3_L2_4YR, T3_L2_5YR; 
    const static cof4 T6_L1_2YR, T6_L1_3YR, T6_L1_4YR, T6_L1_5YR, T6_L1_6YR, T9_L1_2YR, T9_L1_3YR, T9_L1_4YR, T9_L1_5YR, T9_L1_6YR,
		TF_L1_2YR, TF_L1_3YR, TF_L1_4YR, TF_L1_5YR, TF_L1_6YR, T6_L2_2YR, T6_L2_3YR, T6_L2_4YR, T6_L2_5YR, T9_L2_2YR, T9_L2_3YR, T9_L2_4YR, T9_L2_5YR, 
		TF_L2_2YR, TF_L2_3YR, TF_L2_4YR, TF_L2_5YR; 
};
#endif
