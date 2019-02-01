#include "Assumption.hpp"
#include "ReadData.hpp"

#include <iostream>
using namespace std;

Assumption::Assumption(string hpapath, string liborpath, string unemppath, string mortpath, unsigned int numOfMonths, unsigned int trailnum){
	hpa=hpa_vector(hpapath, numOfMonths, trailnum);
    //cout << hpa.size() << endl;
    //for(size_t t = 0; t<hpa.size(); ++t){
    //cout << hpa[t].size() << "|";
    //if(hpa[t].size() != 385) cout << "something wrong 1\n";
    //}
	libor=put_to_double_vec(liborpath);
	unemp=getdata(unemppath);
    mort = put_to_double_vec(mortpath);
}

