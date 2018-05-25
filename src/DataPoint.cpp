/*
 * DataPoint.cpp
 *
 *  Created on: May 24, 2018
 *      Author: soyo
 */

#include "DataPoint.h"

//classes


DataPoint::DataPoint(){
	X = 0.0;
	Y = 0.0;
	isCenter = false;
	nearestCenter = NULL;
	graphNodeIndex = -1;
	indexOfmappedCenter = -1;
	centerID = -1;
}
//DataPoint::~DataPoint() { delete nearestCenter; }

DataPoint::DataPoint(double x, double y){
	X = x;
	Y = y;
	isCenter = false;
	nearestCenter = NULL;
	graphNodeIndex = -1;
	indexOfmappedCenter = -1;
	centerID = -1;
}
double DataPoint::distanceTo(DataPoint p){
	double dx = (X - p.X);
	double dy = (Y - p.Y);
	//return max(dx,dy);
	return sqrt(dx*dx + dy*dy);

}
void DataPoint::print(){
	std::cout <<"....." <<std::endl;
	std::cout <<"p.X:"<< X << "p.Y:" << Y << std::endl;
	std::cout << "p.isCenter: " << isCenter << std::endl;
	std::cout << "p.graphNodeIndex: " << graphNodeIndex << std::endl;
	std::cout << "p.centerID: " << centerID << std::endl;
	std::cout <<"....." <<std::endl;
}



