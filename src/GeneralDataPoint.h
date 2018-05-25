/*
 * GeneralDataPoint.h
 *
 *  Created on: May 24, 2018
 *      Author: soyo
 */

#ifndef GENERALDATAPOINT_H_
#define GENERALDATAPOINT_H_

#include <math.h>
#include <cstdio>
#include <iostream>

template<typename T, int no>
class GeneralDataPoint {

	public:
		T X;
		T Y;
		bool isCenter;
		int graphNodeIndex;
		int indexOfmappedCenter;
		int centerID;
		GeneralDataPoint* nearestCenter;
		GeneralDataPoint();

		GeneralDataPoint(T x[no]);
		double distanceTo(GeneralDataPoint p);
		void print();
	};
#endif /* GENERALDATAPOINT_H_ */
