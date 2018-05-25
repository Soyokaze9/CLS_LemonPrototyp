/*
 * SolverKMedian.h
 *
 *  Created on: May 24, 2018
 *      Author: soyo
 */

#ifndef SOLVERKMEDIAN_H_
#define SOLVERKMEDIAN_H_

#include <lemon/maps.h>
#include <lemon/smart_graph.h>
#include <lemon/list_graph.h>
#include <lemon/capacity_scaling.h>

#include "DataPoint.h"

using namespace lemon;
using namespace std;
DIGRAPH_TYPEDEFS(SmartDigraph);
typedef std::vector<DataPoint> DataVector;

class SolverKMedian {

public:
	//SolverKMedian();
	SolverKMedian(DataVector& ps,int noDP,int k,int cap);
	void printContent();
	virtual ~SolverKMedian();

private:
	DataVector& ps;
	int noDP;
	int k;
	int cap;

	SmartDigraph g;

	//map for cost and capacity
	IntArcMap capacity(SmartDigraph);//(g);

	DoubleArcMap cost(SmartDigraph);//(g);
	IntNodeMap supply(SmartDigraph);//(g);
	//Digraph::NodeMap<string>
	IntArcMap name(SmartDigraph);//(g);
	//StringArcMap name(g);
	Node nodesGraph[]; //noDP+2+k
	Node centerNodes[]; //k

};

#endif /* SOLVERKMEDIAN_H_ */
