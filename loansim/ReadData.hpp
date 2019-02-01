// this is the Input / Output header file
#ifndef READDATA_HPP
#define READDATA_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "LoanDef.h"

using namespace std;

inline bool isNA(double x){return x!=x;}
inline bool IsFiniteNumber(double x){return (x <= DBL_MAX && x >= -DBL_MAX);}

static const unsigned int totalNumOfCodes = 385;
//static const unsigned int totalNumOfCodes = 353;

	//convert string to integer, double, etc
template<class T> 
T conv_to(const string& s);


//convert a string into a vector of double
//return a vector of double
vector<double> put_to_vector(string &linedata); 

//convert a string into a vector of string
//return a vector of string
vector<string> put_to_stringvec(string linedata);
//get a vector of double from a file
//return a vector of vector of double
vector<vector<double> > put_to_double_vec(string ifile);

//get a vector of string from a file
//return the vector
vector<string> read_string(string ifile);

//get a map of <string, vector<double> > from a file, used in unemployment rate assumption
//return a map of <string, vector<double> >
unordered_map<string, vector<double> > getdata(string ifile);
//get a map of <string, vector<string> > from a file
//return a map of <string, vector<string> >
map<string, vector<string> > get_loan_data(string ifile);

//get all loans into a vector of "LoanDef" type from a string
//return the dealinfo structure, vector of loandef type
vector<LoanDef> read_loan_all(ifstream& inFile);

//get a map of <string, vector<vector<double> > > from string, used in mortgage rate assumption, home price appreciation assumption, and coefficients reading
//return a map of <string, vector<vector<double> > >
map<string, vector<vector<double> > > map_double_vec(string ifile);

//write a vector<vector<double> > into file, space delimited
void write_doublevec(string name, vector< vector<double> > data);

//write a map of <string, vector<double> > into file, space delimited
void write_mapvec(string name, map<string, vector<double> > data);

//write a vector<vector<double> > into file, "|" delimited
void write_doublevec(string name, vector< vector<double> > data, unsigned int pre);

//write a map of <string, vector<double> > into file, "|" delimited
void write_doublevec_b(string name, vector< vector<double> > data);

vector<unordered_map<string, vector<double> > > hpa_vector(const string& ifile, const unsigned int numOfMonths, const unsigned int trailnum);

#endif
