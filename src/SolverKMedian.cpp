/*
 * SolverKMedian.cpp
 *
 *  Created on: May 24, 2018
 *      Author: soyo
 */

#include "SolverKMedian.h"

void printVectorL( DataVector v){
    for(DataVector::iterator it=v.begin(); it!=v.end() ; ++it)
            //cout << " " << *it;
    		it->print();
    cout<<endl;
}

void printDPsL(DataPoint ps[], int noDP){
	cout << "+++++dps++++++" << endl;
	for(int k=0;k<noDP;k++){
		cout << "----p:" << k << endl;
		cout << "ps[k].X: " << ps[k].X << endl;
		cout << "ps[k].Y: " << ps[k].Y << endl;
		cout << "ps[k].isCenter: " << ps[k].isCenter << endl;
		cout << "ps[k].graphNodeIndex: " << ps[k].graphNodeIndex << endl;
		cout << "ps[k].centerID: " << ps[k].centerID << endl;
		cout << "----end of p:" << k << endl;
	}
	cout << "+++++dps end++++++"<<endl;
}

/*
SolverKMedian::SolverKMedian() {
	// TODO Auto-generated constructor stub

}
*/
SolverKMedian::SolverKMedian(DataVector& psIn,int noDPIn,int kIn,int capIn):ps(psIn){
	cout << "init psIn&:" << &psIn <<endl;
	//ps(psIn);
	cout << "init after ps&:" << &ps <<endl;
	noDP = noDPIn;
	k = kIn;
	cap = capIn;
	cout << "init end psIn&:" << &psIn <<endl;
};

SolverKMedian::~SolverKMedian() {
	// TODO Auto-generated destructor stub
}

void SolverKMedian::printContent(){
	cout << "noDP:" << noDP <<endl;
	cout << "k:" << k <<endl;
	cout << "cap:" << cap <<endl;
	cout << "ps&:" << &ps <<endl;
	printVectorL(ps);
	//printDPsL(ps,noDP);
}
