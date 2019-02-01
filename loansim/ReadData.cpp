#include "ReadData.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <cstdlib>
#include <cstdio>
#include <cstring>
//#include "string.h"
//#include <cstddef>
#include "LoanDef.h"
#include "sse_mathfun.h"

using namespace std;

//static const unsigned int totalNumOfCodes = 385;

//convert string to integer, double, etc
//too slow compared to naive approach
/*
template<class T> 
T conv_to(const string& s){
    T t;
    istringstream(s) >> t;
    return t;
}
*/

//convert a string into a vector of double
//return a vector of double
vector<double> put_to_vector(string &linedata) {
    vector<double> res;
    int pos, posx, len;
    double num;
    string str;

    len = linedata.length();
    posx = -1;

    for(int i=0; i<len; i++) {
        if(linedata.at(i)==' ') {
            pos = i;
            str = linedata.substr(posx+1, pos-posx-1);
            posx = pos;
            num = naive_atof(str.c_str());
            res.push_back(num);
        }
    }
    str = linedata.substr(posx+1, len-posx-1);
    num = naive_atof(str.c_str());
    res.push_back(num);
    return res;
}

//convert a string into a vector of string
//return a vector of string
vector<string> put_to_stringvec(string linedata) {
    vector<string> res;
    int pos, posx, len;
    string str;

    len = linedata.length();
    posx = -1;

    for(int i=0; i<len; i++) {
        if(linedata.at(i)==' '|| linedata.at(i)=='\t') {
            pos = i;
            str = linedata.substr(posx+1, pos-posx-1);
            posx = pos;
            res.push_back(str);
        }
    }
    str = linedata.substr(posx+1, len-posx-1);
    res.push_back(str);
    return res;
}

//get a vector of double from a file
//return a vector of vector of double
vector<vector<double> > put_to_double_vec(string ifile) {
    vector<vector<double> > middata;
    vector<double> temp;
    string str;

    ifstream inFile(ifile.c_str());
    if (!inFile) {
        cerr<<"Unable to open file: "<<ifile<<endl;
    }
    while (getline(inFile, str)) {
        temp = put_to_vector(str);
        middata.push_back(temp);
    }
    return middata;
}

//get a vector of string from a file
//return the vector
vector<string> read_string(string ifile) {
    vector<string> all_value;   // this is used to store the data
    ifstream inFile(ifile.c_str());
    string str;

    if (!inFile) {
        cerr<<"Unable to open file: "<<ifile<<endl;
    }
    while (getline(inFile, str)) {
        all_value.push_back(str);
    }
    return all_value;
}

//get a unordered_map of <string, vector<double> > from a file, used in unemployment rate assumption
//return a unordered_map of <string, vector<double> >
unordered_map<string, vector<double> > getdata(string ifile) {
    unordered_map<string, vector<double> > all_value;   // this is used to store the data
    string str, headstr, tailstr;
    int i;
    vector<double> temp;

    ifstream inFile(ifile.c_str());

    if (!inFile) {
        cerr<<"Unable to open file: "<<ifile<<endl;
    }
    while (getline(inFile, str)) {
        for(i=0; str.at(i)!=' '; i++) {}
        headstr = str.substr(0,i);                      //used to get the name
        tailstr = str.substr(i+1,str.length()-i);       //used to get the body
        temp = put_to_vector(tailstr);
        all_value.insert(pair<string, vector<double> >(headstr, temp));
    }
    return all_value;
}

//get a unordered_map of <string, vector<string> > from a file
//return a unordered_map of <string, vector<string> >
map<string, vector<string> > get_loan_data(string ifile) {
    // this is used to put all string
    map<string, vector<string> > all_value;   // this is used to store the data
    string str, headstr, tailstr;
    int i;
    vector<string> temp;

    ifstream inFile(ifile.c_str());

    if (!inFile) {
        cerr<<"Unable to open file: "<<ifile<<endl;
    }
    while (getline(inFile, str)) {
        for(i=0; str.at(i)!=' '; i++) {}
        headstr = str.substr(0,i);                      //used to get the name
        tailstr = str.substr(i+1,str.length()-i);       //used to get the body
        temp = put_to_stringvec(tailstr);
        all_value.insert(pair<string, vector<string> >(headstr, temp));
    }
    return all_value;
}

//get all loans into a vector of "LoanDef" type from a string
//return the dealinfo structure, vector of loandef type
vector<LoanDef> read_loan_all(ifstream& inFile){
    // this is used to put all string
    vector<LoanDef> all_value;  // this is used to store the data
    string line;

    vector<const char*> temp;
    const char delimiters[] = " ";
    char *running;
    char *token;
    amort_data am;

    //TODO: configure it in makefile, 
    //strdupa is not available in MINGW, so is strsep

    while (getline(inFile, line)) {
        running = strdup(line.c_str());
        token = strtok(running, delimiters);
        temp.push_back(token);
        //running = strdup(line.c_str());
        //while(strsep(&running, delimiters))!=NULL){
        while(token!=NULL){
            //printf("%s\n", token);
            token = strtok(NULL, delimiters);
            temp.push_back(token);
        }
        //printf("%d\n", temp.size());
        //temp = put_to_stringvec(str);
        LoanDef loanmid(
            temp[0],
            temp[1],
            temp[2],
            naive_atof(temp[3]),
            temp[4][0],
            temp[5][0],
            temp[6][0],
            temp[7][0],
            naive_atoi(temp[8]),
            naive_atoi(temp[9]),
            naive_atof(temp[10]),
            naive_atof(temp[11]),
            temp[12][0],
            naive_atoi(temp[13]), 
            naive_atof(temp[14]), 
            naive_atof(temp[15]), 
            naive_atof(temp[16]),
            naive_atoi(temp[17]),
            naive_atof(temp[18]), 
            naive_atof(temp[19]), 
            naive_atof(temp[20]), 
            naive_atof(temp[21]), 
            naive_atof(temp[22]),
            naive_atof(temp[23]), 
            naive_atof(temp[24]), 
            naive_atof(temp[25]), 
            naive_atof(temp[26]), 
            naive_atoi(temp[27]),
            naive_atoi(temp[28]), 
            naive_atof(temp[36]), 
            naive_atof(temp[37]),  // **these two are for Negative Amortization loans, they are the negative amortization limit and the minimum payment
            naive_atof(temp[29]), 
            naive_atof(temp[30]), 
            naive_atof(temp[31]), 
            naive_atof(temp[32]),
            0,  //default amount
            temp[33], 
            naive_atof(temp[34]), 
            naive_atoi(temp[35]),
            naive_atof(temp[38]), 
            naive_atof(temp[39]),
            naive_atof(temp[40]), 
            naive_atoi(temp[41]),
            naive_atof(temp[42]), 
            naive_atoi(temp[43]),
            naive_atof(temp[44]), 
            naive_atoi(temp[45]), 
            naive_atoi(temp[46]), 
            naive_atof(temp[47]),
            temp[48], temp[49]);  // unemployment rate, season and initial rate
        temp.clear();
        loanmid.change_init(0.0);
        all_value.push_back(loanmid);
    }
    return all_value;
}

//get a unordered_map of <string, vector<vector<double> > > from string, used in mortgage rate assumption, home price appreciation assumption, and coefficients reading
//return a unordered_map of <string, vector<vector<double> > >
map<string, vector<vector<double> > > map_double_vec(string ifile){
    // this is used to convert all to string
    map<string, vector<vector<double> > > all_value;  // this is used to store the data
    string str, headstr, tailstr;
    int i, flag;
    vector<double> temp;
    vector<vector<double> > tempx;
    map<string, vector<vector<double> > >::iterator iterm;

    ifstream inFile(ifile.c_str());

    if (!inFile) {
        cerr<<"Unable to open file: "<<ifile<<endl;
    }
    while (getline(inFile, str)) {
        flag = 0;
        for(i=0; str.at(i)!=' '; i++) {}
        headstr = str.substr(0,i);                      //used to get the name
        tailstr = str.substr(i+1,str.length()-i);       //used to get the body
        temp = put_to_vector(tailstr);
        
        for (iterm=all_value.begin(); iterm!=all_value.end(); iterm++) {
            if (iterm->first == headstr) {
                flag = 1;
            }
        }
        if (flag == 0) {
            tempx.push_back(temp);
            all_value.insert(pair<string, vector<vector<double> > >(headstr, tempx));
        }
        else {
            all_value.find(headstr)->second.push_back(temp);
        }
        tempx.clear();
        temp.clear();
    }

    return all_value;
}

//write a vector<vector<double> > into file, space delimited
void write_doublevec(string name, vector< vector<double> > data) {
    ofstream outfile(name.c_str(), ios::out);
    outfile.setf(ios::fixed, ios::floatfield); 
    outfile.precision(2);
    vector< vector<double> >::iterator iter1;
    vector<double>::iterator iter2;
    for (iter1=data.begin(); iter1!=data.end(); iter1++) {
        for(iter2=(*iter1).begin(); iter2!=(*iter1).end(); iter2++) {
            outfile<<*iter2<<' ';
        }
    outfile<<'\n';
    }
    outfile.close();
}

//write a vector<vector<double> > into file, "|" delimited
void write_doublevec(string name, vector< vector<double> > data, unsigned int pre){
    ofstream outfile(name.c_str(), ios::out);
    outfile.setf(ios::fixed, ios::floatfield); 
    outfile.precision(pre);
    vector< vector<double> >::iterator iter1;
    vector<double>::iterator iter2;
    for (iter1=data.begin(); iter1!=data.end(); ++iter1) {
        for(iter2=(*iter1).begin(); iter2!=iter1->end(); ++iter2) {
            outfile<<*iter2<<'|';
        }
    outfile<<'\n';
    }
    outfile.close();
}

//write a unordered_map of <string, vector<double> > into file, "|" delimited
void write_doublevec_b(string name, vector< vector<double> > data) {
    ofstream outfile(name.c_str(), ios::out);
    outfile.setf(ios::fixed, ios::floatfield); 
    outfile.precision(3);
    vector< vector<double> >::iterator iter1;
    vector<double>::iterator iter2;
    double mid, midx;
    for (iter1=data.begin(); iter1!=data.end(); iter1++) {
        midx=0.38;
        for(iter2=(*iter1).begin(); iter2!=(*iter1).end(); iter2++) {
            mid = *iter2;
            if(mid==0) {outfile<<midx<<'|';}
            else {
                outfile<<mid<<'|';
                midx=mid;
            }
        }
        outfile<<'\n';
    }
    outfile.close();
}

vector<unordered_map<string, vector<double> > > hpa_vector(const string& ifile, const unsigned int numOfMonths, const unsigned int trailnum) {
// this is used to put all string
    vector<unordered_map<string, vector<double> > > all_value;  // this is used to store the data
    string str, headstr, tailstr;
    vector<double> temp;
    //temp.resize(361);

    unordered_map<string, vector<double> > tempx;
    //tempx.resize(300);

    FILE *file = fopen(ifile.c_str(), "r");
    if(file != NULL){

        //assume 2175 which is [360 + 1(first month) + 1(code) ]*6 + 1 (null)
        char line[2180];
        //char line[6*numOfMonths+13];
        char tmp[6]="00000";
        //for(
        for(unsigned int i = 0; i!= trailnum; ++i){
            for(unsigned int j = 0; j!=totalNumOfCodes; ++j){
                //if(fgets(line, sizeof line, file)!=NULL){
                //if(fgets(line, numOfMonths*6+13, file)!=NULL){
                if(fgets(line, 2180, file)!=NULL){
                    //printf("%s\n", line);
                    for(size_t t=1; t!=numOfMonths+2; ++t){ 
                        temp.push_back(naive_atof(strncpy(tmp, line+6*t,5)));
                        //cout << naive_atof(strncpy(tmp, line+6*t,5)) << endl;
                    }
                    tempx.insert(pair<string, vector<double> >(string(strncpy(tmp, line,5)), temp));
                    //cout << string(strncpy(tmp, line,5)) << "\n";
                    //cout << temp.front() << "|" << temp.back() << "|" << temp.size() << "\n";
                    temp.clear();
                }
                else {
                    printf("Checking HPA file\n");
                }
            }
            all_value.push_back(tempx);
            tempx.clear();
        }
        fclose(file);
    }

    else {
        perror(ifile.c_str());
    }

    //cout << "size of hpa_vector is: " << all_value.size() << endl;
    //for(vector<unordered_map<string, vector<double> > >::iterator it = all_value.begin(); it!=all_value.end(); ++it){
    //cout << "size of hpa is: " << it->size() << endl;
    //}
    return all_value;
}
