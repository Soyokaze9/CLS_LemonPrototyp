/*
 * GeneralDataPoint.cpp
 *
 *  Created on: May 24, 2018
 *      Author: soyo
 */

#include "GeneralDataPoint.h"


template<class T, int no>
GeneralDataPoint<T,no>::GeneralDataPoint(){

}

template<class T, int no>
GeneralDataPoint<T,no>::GeneralDataPoint(T x[no]){

}

template<class T, int no>
double GeneralDataPoint<T,no>::distanceTo(GeneralDataPoint p){

}

template<class T, int no>
void GeneralDataPoint<T,no>::print(){}

/*
GeneralDataPoint::GeneralDataPoint(){
	X = 0.0;
	Y = 0.0;
	isCenter = false;
	nearestCenter = NULL;
	graphNodeIndex = -1;
	indexOfmappedCenter = -1;
	centerID = -1;
}

GeneralDataPoint::GeneralDataPoint(double x, double y){
	X = x;
	Y = y;
	isCenter = false;
	nearestCenter = NULL;
	graphNodeIndex = -1;
	indexOfmappedCenter = -1;
	centerID = -1;
}
double GeneralDataPoint::distanceTo(GeneralDataPoint p){
	double dx = (X - p.X);
	double dy = (Y - p.Y);
	//return max(dx,dy);
	return sqrt(dx*dx + dy*dy);

}
void GeneralDataPoint::print(){
	std::cout <<"....." <<std::endl;
	std::cout <<"p.X:"<< X << "p.Y:" << Y << std::endl;
	std::cout << "p.isCenter: " << isCenter << std::endl;
	std::cout << "p.graphNodeIndex: " << graphNodeIndex << std::endl;
	std::cout << "p.centerID: " << centerID << std::endl;
	std::cout <<"....." <<std::endl;
}
*/

