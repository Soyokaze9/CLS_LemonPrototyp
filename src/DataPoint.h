/*
 * DataPoint.h
 *
 *  Created on: May 24, 2018
 *      Author: soyo
 */

#ifndef DATAPOINT_H_
#define DATAPOINT_H_

#include <math.h>
#include <cstdio>
#include <iostream>

//classes
class DataPoint{

public:
	double X;
	double Y;
	bool isCenter;
	int graphNodeIndex;
	int indexOfmappedCenter;
	int centerID;
	DataPoint* nearestCenter;
	DataPoint();
	virtual ~DataPoint() { delete nearestCenter; }

	DataPoint(double x, double y);
	double distanceTo(DataPoint p);
	void print();
};


#endif /* DATAPOINT_H_ */

