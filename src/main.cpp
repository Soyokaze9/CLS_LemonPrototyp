#include <iostream>

#include <cstdio>
#include <fstream>

#include <string>
#include <list>
#include <float.h>

#include <lemon/smart_graph.h>

#include <lemon/maps.h>
#include <lemon/list_graph.h>
#include <lemon/capacity_scaling.h>
#include <lemon/network_simplex.h>
#include <lemon/preflow.h>
#include <lemon/lgf_reader.h>
#include <lemon/lgf_writer.h>
#include <lemon/dimacs.h>

#include "filereader.hpp"

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
	DataPoint(){
		X = 0;
		Y = 0;
		isCenter = false;
		nearestCenter = NULL;
		graphNodeIndex = -1;
		indexOfmappedCenter = -1;
		centerID = -1;
	}
	~DataPoint() { delete nearestCenter; }

	DataPoint(double x, double y){
		X = x;
		Y = y;
		isCenter = false;
		nearestCenter = NULL;
		graphNodeIndex = -1;
		indexOfmappedCenter = -1;
		centerID = -1;
	}
	double distanceTo(DataPoint p){
		double dx = abs(X - p.X);
		double dy = abs(Y - p.Y);
		//return max(dx,dy);
		return sqrt(dx*dx + dy*dy);
	}
};

//int example1();
//int example2();
//int example3();
void exampleSearch();
void exampleSearchBF();
void simpleExampleSearchBF();
void mediumExampleSearchBF();
void exampleLoopWithPlot();
double localSearch(DataPoint ps[], int noDP, int k, int cap);
double bruteForceSearch(DataPoint ps[], int noDP, int k, int cap);
void directPlot(DataPoint* dps, int noDP);
void directPlotRegions(DataPoint* dps, int noDP,int centers[], int noCenters,int regions[],const char * title);
void directPlotPoints(DataPoint* dps, int noDP,int centers[], int noCenters,const char * title);
double calcRegions(DataPoint ps[],int cPoints[], int noDP, int k, int cap, int regions[]);
double calcRegionsCS(DataPoint ps[],int cPoints[], int noDP, int k, int cap, int regions[]);

std::string vertNoToString(int vertexNo,int maxNo);
void printList( std::list<int> l);
void printList( std::list<std::list<int>> l);
void printVector( std::vector< std::vector<int> > v);
void subset(int arr[], int size, int left, int index, std::list<int> &l, std::list<std::list<int>> &all);


using namespace lemon;
using namespace std;

//main
int main() {
	/*
	int no = 5;
    double xvals[no] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double yvals[no] = {5.0 ,3.0, 1.0, 3.0, 5.0};
    DataPoint dps[no];
    for(int i=0;i<no;i++){
    	dps[i] = DataPoint(xvals[i],yvals[i]);
    }
	 */
	//exampleLoopWithPlot();

	/*
    int array[5]={1,2,3,4,5};
    std::list<int> lt;
    std::list<std::list<int>> all;
    subset(array,5,3,0,lt,all);
    cout << "ALL--------------" << endl;
    printList(all);
    for(list<list<int>>::iterator it=l.begin(); it!=l.end() ; ++it)
         std::list<int> l = (*it) ;

	 */

	Data dataBucket;
	//readIntsFromFile("filereader-test",dataBucket);//char del optional;
	readIntsFromFile("./res/filereader-test",dataBucket);
	cout << "main" << endl;
	printVector(dataBucket);
	cout << "after pv" << endl;
	// Output to see if everything was read correctly
	for(std::size_t i=0; i < dataBucket.size(); ++i){
		for(std::size_t j=0; j < dataBucket[i].size(); ++j){
			std::cout << std::setw(5) << dataBucket[i][j] << " ";
		}
		std::cout << std::endl;
	}

	//exampleSearch();
	//mediumExampleSearchBF();
	return 0;
}

//examples ******************
void exampleSearchBF(){
	int noDP = 8;
	int k = 3;
	int cap = 2;
	DataPoint p1(1,2);
	DataPoint p2(1,5);
	DataPoint p3(2,1);
	DataPoint p4(2,2);
	DataPoint p5(2,3);
	DataPoint p6(2,5);
	DataPoint p7(6,2);
	DataPoint p8(7,2);
	//p4.isCenter = true;
	//p6.isCenter = true;
	//p7.isCenter = true;
	DataPoint ps[noDP] = {p1,p2,p3,p4,p5,p6,p7,p8};
	bruteForceSearch(ps, noDP,  k,  cap);
}

void simpleExampleSearchBF(){

	int noDP = 3;
	int k = 1;
	int cap = 2;
	DataPoint p1(1,1);
	DataPoint p2(2,1);
	DataPoint p3(3,1);
	DataPoint ps[noDP] = {p1,p2,p3};

	bruteForceSearch(ps, noDP,  k,  cap);
}

void mediumExampleSearchBF(){

	int noDP = 6;
	int k = 2;
	int cap = 2;
	DataPoint p1(1,1);
	DataPoint p2(2,1);
	DataPoint p3(3,1);

	DataPoint p4(1,3);
	DataPoint p5(2,3);
	DataPoint p6(3,3);


	DataPoint ps[noDP] = {p1,p2,p3,p4,p5,p6};

	bruteForceSearch(ps, noDP,  k,  cap);

	localSearch(ps, noDP,  k,  cap);

}

void exampleSearch(){
	int noDP = 8;
	int k = 3;
	int cap = 2;
	DataPoint p1(1,2);
	DataPoint p2(1,5);
	DataPoint p3(2,1);
	DataPoint p4(2,2);
	DataPoint p5(2,3);
	DataPoint p6(2,5);
	DataPoint p7(6,2);
	DataPoint p8(7,2);
	//p4.isCenter = true;
	//p6.isCenter = true;
	//p7.isCenter = true;
	DataPoint ps[noDP] = {p1,p2,p3,p4,p5,p6,p7,p8};
	localSearch(ps, noDP,  k,  cap);
}

void exampleLoopWithPlot(){
	int noDP = 8;
	int k = 3;
	int cap = 2;
	DataPoint p1(1,2);
	DataPoint p2(1,5);
	DataPoint p3(2,1);
	DataPoint p4(2,2);
	DataPoint p5(2,3);
	DataPoint p6(2,5);
	DataPoint p7(6,2);
	DataPoint p8(7,2);
	p4.isCenter = true;
	p4.centerID = 1;
	p6.isCenter = true;
	p6.centerID = 2;
	p7.isCenter = true;
	p7.centerID = 3;
	DataPoint ps[noDP] = {p1,p2,p3,p4,p5,p6,p7,p8};
	int cPoints[k];
	int counter = 0;
	for(int i=0;i<noDP;i++){
		if (ps[i].isCenter){
			cPoints[counter]=i;
			counter++;
		}
	}
	int regions[noDP];
	double newCost = calcRegions(ps,cPoints, noDP, k,cap,regions);
	directPlotPoints(ps, noDP,cPoints, k, "Example");
	directPlotRegions(ps, noDP,cPoints, k,regions,"Example");
}

/*
int example1(){
    int noDP = 8;
    int k = 3;
    int cap = 2;
	DataPoint p1(1,2);
    DataPoint p2(1,5);
    DataPoint p3(2,1);
    DataPoint p4(2,2);
    DataPoint p5(2,3);
    DataPoint p6(2,5);
    DataPoint p7(6,2);
    DataPoint p8(7,2);
    p4.isCenter = true;
    p6.isCenter = true;
    p7.isCenter = true;
    DataPoint ps[noDP] = {p1,p2,p3,p4,p5,p6,p7,p8};
    int regions[noDP];
    DataPoint cPoints[k];
    int counter = 0;
    for(int i=0;i<noDP;i++){
    	if (ps[i].isCenter){
    		cPoints[counter]=ps[i];
			counter++;
    	}
    }

    return calcRegions(ps,cPoints, noDP, k,cap,regions);
}

int example2(){
    int noDP = 3;
    int k = 2;
    int cap = 1;
	DataPoint p1(1,2);
    DataPoint p2(2,2);
    DataPoint p3(4,2);
    p1.isCenter = true;
    p3.isCenter = true;
    DataPoint ps[noDP] = {p1,p2,p3};
    DataPoint cPoints[k];
    int counter = 0;
    for(int i=0;i<noDP;i++){
    	if (ps[i].isCenter){
    		cPoints[counter]=ps[i];
			counter++;
    	}
    }
    int regions[noDP];
    return calcRegions(ps, cPoints, noDP, k,cap,regions);
}

int example3(){
    int noDP = 5;
    int k = 2;
    int cap = 2;
	DataPoint p1(1,2);
    DataPoint p2(2,2); //p2(4,3);
    DataPoint p3(4,2);
    DataPoint p4(1,3);
    DataPoint p5(3,2);
    p1.isCenter = true;
    p3.isCenter = true;
    DataPoint ps[noDP] = {p1,p2,p3,p4,p5};
    int regions[noDP];
    DataPoint cPoints[k];
    int counter = 0;
    for(int i=0;i<noDP;i++){
    	if (ps[i].isCenter){
    		cPoints[counter]=ps[i];
			counter++;
    	}
    }
    calcRegions(ps, cPoints, noDP, k,cap,regions);
    printf("new region\n");
    for(int i=0;i<noDP;i++){
    	//printf("Region Map %d->%d\n");
    	printf("index %d value %d\n",i+1,(regions[i]+1)>noDP?0:regions[i]+1);
    }

    return 0;
}
 */
//end of examples ******************


void printDPs(DataPoint ps[], int noDP){
	cout << "+++++dps++++++" << endl;
	for(int k=0;k<noDP;k++){
		cout << "----p:" << k << endl;
		cout << "ps[k].isCenter: " << ps[k].isCenter << endl;
		cout << "ps[k].graphNodeIndex: " << ps[k].graphNodeIndex << endl;
		cout << "ps[k].centerID: " << ps[k].centerID << endl;
		cout << "----end of p:" << k << endl;
	}
	cout << "+++++dps end++++++"<<endl;
}

//local search:
// IN:
// ps[] - Data, noDP - number of DataPoints, k - number of medians, cap - Capacity of median
//
double localSearch(DataPoint ps[], int noDP, int k, int cap){
	//start with random centers
	int cPoints[k];
	int cPointsNew[k];
	int regions[noDP];
	int regionsNew[noDP];

	bool beVerbose = true;
	srand (time(NULL));
	//seed:1526051006
	srand (1526051006);

	cout << "srand seed:" << time(NULL) << endl;
	for(int i=0;i<k;i++){
		bool notFound = true;
		while(notFound){
			int randVal = rand();
			cout <<"rnd:"<< randVal <<endl;
			int randInt = randVal % noDP;
			cout <<"randInt:"<< randInt <<endl;

			if(!ps[randInt].isCenter){
				ps[randInt].isCenter = true;
				ps[randInt].centerID = i;
				cPoints[i] = randInt;
				cPointsNew[i] = randInt;
				cout << "cPoints[i]: x " << ps[cPoints[i]].X << " y:"<< ps[cPoints[i]].Y << endl;
				notFound = false;
			}
		}
	}


	//cout << "In LS start calc" << endl;
	//printDPs(ps, noDP);
	double currentCost = calcRegionsCS(ps, cPoints, noDP, k,cap,regions);
	//cout << "In LS after first calc" << endl;
	//printDPs(ps, noDP);

	double newCost;
	double improvement = currentCost;
	double minImprovment = 0.01;
	printf("currentCost: %f",currentCost);

	std::ostringstream stringStream;
	stringStream << "Start with cost:" << currentCost;
	//stringf();
	std::string formatedStr = stringStream.str();
	const char* cstr = formatedStr.c_str();

	//directPlot(ps, noDP);
	//directPlotPoints(ps, noDP,cPoints, k, "Start");
	directPlotRegions(ps, noDP,cPoints, k,regions,cstr);

	//cout << "After print" << endl;
	//printDPs(ps, noDP);

	bool forBreak = false;
	int iterations = 0;
	while(iterations < 5 && (improvement>minImprovment)){
		if(beVerbose)cout <<" " << endl;
		if(beVerbose)cout <<"iteration:  "<<iterations << endl;
		if(beVerbose)cout <<"start for j " << endl;

		for(int j=0;j<k;j++){
			if(beVerbose)cout <<"j =  "<<j << endl;
			for(int l=0;l<noDP;l++){
				if(beVerbose)cout <<"trying l =  "<<l << endl;
				//try swap if not already center
				if(!ps[l].isCenter){
					cPointsNew[j] = l;
					ps[cPoints[j]].isCenter = false;
					ps[cPointsNew[j]].isCenter = true;
					ps[cPointsNew[j]].centerID = ps[cPoints[j]].centerID;
					//ps[cPoints[j]].centerID = -1;
					cout <<"---after try swap --\n";
					//cout << "ps[l] isCenter expected 1 :" << ps[l].isCenter << endl;
					//cout << "ps[cPoints[j]].isCenter expected 0:" << ps[cPoints[j]].isCenter << endl;
					//cout << "ps[cPoints[j].graphNodeIndex].isCenter expected 0:" << ps[ps[cPoints[j]].graphNodeIndex].isCenter << endl;
					//cout <<"---end try--\n";

					cout << "After switch" << endl;
					cout << "In LS before iter calc" << endl;
					newCost = calcRegionsCS(ps, cPoints, noDP, k,cap,regions);
					//cout << "In LS after iter calc" << endl;
					//printDPs(ps, noDP);

					cout << "newCost:" << newCost << endl;
					cout << "currentCost:" << currentCost << endl;

					if(newCost<currentCost){
						if(beVerbose)cout <<"found better solution at j:" << j <<" l:" << l << endl;
						cout << "switched: " << cPoints[j] << "with " << l << endl;
						ps[cPoints[j]].centerID = -1;
						cPoints[j] = cPointsNew[j];

						improvement = currentCost-newCost;
						currentCost=newCost;
						forBreak = true;

						std::ostringstream stringStream;
						stringStream << "Iteration: "  << iterations <<" with cost:"<<currentCost;
						std::string formatedStr = stringStream.str();
						const char* cstr = formatedStr.c_str();
						directPlotRegions(ps, noDP,cPoints, k,regions,cstr);


						break;
						//goto stop;
					}else{
						ps[cPoints[j]].isCenter = true;
						ps[cPointsNew[j]].isCenter = false;
						ps[cPointsNew[j]].centerID = -1;
						cout <<"++++swap back+++\n";
						//cout << "cPoints[j].isCenter exp 1:" << ps[cPoints[j]].isCenter << endl;
						//cout << "cPointsNew[j].isCenter exp 0:" << ps[cPointsNew[j]].isCenter << endl;
						//cout << "ps[cPoints[j].graphNodeIndex] isCenter exp 1:" << ps[ps[cPoints[j]].graphNodeIndex].isCenter << endl;
						//cout << "ps[l] isCenter expected 0 :" << ps[l].isCenter << endl;
						//cout <<"++++end back+++\n";
						cPointsNew[j] = cPoints[j];

						//cout << "After back switch" << endl;
						//printDPs(ps, noDP);

					}

				}

			} // for l = current point to swap in for Center j
			if(forBreak){
				if(beVerbose)cout <<"break out of j"<< endl;
				cout <<"break out of j"<< endl;
				break;
			}
		} //for j= current Center to switch
		iterations++;
		cout << "After iteration:" << iterations << endl;
		if(forBreak)forBreak = false;
		else improvement = 0;

	}
	cout <<  "While Loop exited at iter:" << iterations <<endl;
	stringStream << "MinCost found:" <<currentCost<<endl;
	return 0;
}



//returns cost of mapping to given centers, regions is map #index of DP to index in DP of center
double calcRegions(DataPoint ps[],int cPointsInds[], int noDP, int k, int cap, int regions[]){


	bool beVerbose = false;
	bool lilVerbose = true;

	if(beVerbose||lilVerbose)printf("*****calcBegin********\n");

	//create graph
	DIGRAPH_TYPEDEFS(SmartDigraph);
	SmartDigraph g;
	//map for cost and capacity
	IntArcMap capacity(g);
	DoubleArcMap cost(g);
	IntNodeMap supply(g);
	//Digraph::NodeMap<string>
	IntArcMap name(g);
	//StringArcMap name(g);
	Node nodesGraph[noDP+2];

	//for all points
	//add vertex
	for(int i=0;i<noDP;i++){
		ps[i].graphNodeIndex = i;
		nodesGraph[i] = g.addNode();
		supply[nodesGraph[i]] = 0;
	}
	//make note of centers for later
	int cPoints[k];

	int counter = 0;
	for(int i=0;i<noDP;i++){
		if (ps[i].isCenter){
			cPoints[counter]=i;
			counter++;
		}
	}

	if(counter<k){
		cout << "NOT ENOUGH CENTERS" << endl;
		return 0;
	}


	//cout << "In Calc after Nodes init" << endl;
	//printDPs(ps, noDP);
	// add s and t
	nodesGraph[noDP] = g.addNode();
	nodesGraph[noDP+1] = g.addNode();
	int indexS = noDP;
	int indexT = noDP+1;

	int demand = (noDP-k);
	if(beVerbose)cout << "demand:" << demand << endl;
	supply[nodesGraph[indexS]] = (demand);
	supply[nodesGraph[indexT]] = -(demand);

	int numberOfArcs = (noDP-k)*(k+1)+k;
	if(beVerbose)cout << "numberOfArcs:" << numberOfArcs << endl;

	int arcCounter = 0;
	//for all points
	for(int i=0;i<noDP;i++){
		Arc newArc;
		Node pnode;
		if (ps[i].isCenter){
			//// add arc c->t , cost 0, flowcap cap
			pnode = nodesGraph[ps[i].graphNodeIndex];
			newArc = g.addArc(pnode,nodesGraph[indexT]);
			capacity[newArc] = cap;
			cost[newArc] = 0;
			arcCounter++;
			//arcNames[arcCounter] = "c->t";//string("c: node") + std::to_string(i) + "-> t";
		}else{
			//// add arc s->p , cost 0, flowcap 1
			pnode = nodesGraph[ps[i].graphNodeIndex];
			newArc = g.addArc(nodesGraph[indexS],pnode);
			capacity[newArc] = 1;
			cost[newArc] = 0;
			arcCounter++;
			//arcNames[arcCounter] = "s->p";//string("S -> node") + i;
			//forall c add p->c cost dist, flowcap 1
			for(int j=0;j<k;j++){
				Node cnode = nodesGraph[ps[cPoints[j]].graphNodeIndex];
				newArc = g.addArc(pnode, cnode);
				if(beVerbose)cout << g.id(g.source(newArc)) << "->" << g.id(g.target(newArc))<< endl;
				capacity[newArc] = 1;
				cost[newArc] = ps[i].distanceTo(ps[cPoints[j]]);
				arcCounter++;
				//string tempS = "p->c";string("p: node") + i +"-> c: node "+ j +" cost:" + ps[i].distanceTo(cPoints[j]);
				//arcNames[arcCounter] ="p->c";;
			}
		}
	}
	if(beVerbose||lilVerbose)printf("Input graph created with %d nodes and %d arcs\n\n",
			countNodes(g), countArcs(g));
	// Initialize NetworkSimplex algorithm object
	NetworkSimplex<SmartDigraph> ns(g);
	//CapacityScaling<SmartDigraph> ns(g);

	//cout << "In Calc before run" << endl;
	//printDPs(ps, noDP);

	ns.upperMap(capacity).costMap(cost).supplyMap(supply);
	// Run NetworkSimplex
	ns.run();
	// Print total flow cost
	if(beVerbose||lilVerbose)printf("Total flow cost: %d\n\n", ns.totalCost());

	// Print flow values on the arcs with ArcIterator
	if(beVerbose)printf("Flow values on arcs:\n");
	for (ArcIt a(g); a != INVALID; ++a) {
		//if it is not s or t
		if(ns.flow(a)>0){
			if(g.id(g.source(a))<noDP){
				if(g.id(g.target(a))<noDP){
					if(beVerbose)printf("Region Map %d->%d\n",g.id(g.source(a))+1 ,g.id(g.target(a))+1);
					regions[g.id(g.source(a))] = g.id(g.target(a));
				}else{
					if(beVerbose)printf("Region Map %d-is Center\n",g.id(g.source(a))+1);
					regions[g.id(g.source(a))] = g.id(g.source(a));
				}
			}
		}
		if(beVerbose)printf("Arc %d: %s->%s:  %d/%d cost: %lf\n", g.id(a),
				vertNoToString(g.id(g.source(a)),noDP).c_str(),
				vertNoToString(g.id(g.target(a)),noDP).c_str(),
				ns.flow(a), capacity[a] ,cost[a]);
	}
	for(int i=0;i<noDP;i++){
		//printf("Region Map %d->%d\n");
		if(beVerbose)printf("index %d value %d\n",i+1,regions[i]+1);
	}

	//cout << "In Calc return " << endl;
	//printDPs(ps, noDP);
	if(beVerbose||lilVerbose)printf("*****calcEnd********\n");
	return ns.totalCost();
}

//returns cost of mapping to given centers, regions is map #index of DP to index in DP of center
double calcRegionsCS(DataPoint ps[],int cPointsInds[], int noDP, int k, int cap, int regions[]){

	bool beVerbose = true;
	bool lilVerbose = true;
	if(beVerbose||lilVerbose)printf("*****calcBegin********\n");
	//create graph
	DIGRAPH_TYPEDEFS(SmartDigraph);
	SmartDigraph g;
	//map for cost and capacity
	IntArcMap capacity(g);
	DoubleArcMap cost(g);
	IntNodeMap supply(g);
	//Digraph::NodeMap<string>
	IntArcMap name(g);
	//StringArcMap name(g);
	Node nodesGraph[noDP+2];

	//for all points
	//add vertex
	for(int i=0;i<noDP;i++){
		ps[i].graphNodeIndex = i;
		nodesGraph[i] = g.addNode();
		supply[nodesGraph[i]] = 0;
	}
	//make note of centers for later
	int cPoints[k];

	int counter = 0;
	for(int i=0;i<noDP;i++){
		if (ps[i].isCenter){
			cPoints[counter]=i;
			counter++;
		}
	}

	if(counter<k){
		cout << "NOT ENOUGH CENTERS" << endl;
		return 0;
	}


	//cout << "In Calc after Nodes init" << endl;
	//printDPs(ps, noDP);
	// add s and t
	nodesGraph[noDP] = g.addNode();
	nodesGraph[noDP+1] = g.addNode();
	int indexS = noDP;
	int indexT = noDP+1;

	int demand = (noDP-k);
	if(beVerbose)cout << "demand:" << demand << endl;
	supply[nodesGraph[indexS]] = (demand);
	supply[nodesGraph[indexT]] = -(demand);

	int numberOfArcs = (noDP-k)*(k+1)+k;
	if(beVerbose)cout << "numberOfArcs:" << numberOfArcs << endl;

	int arcCounter = 0;
	//for all points
	for(int i=0;i<noDP;i++){
		Arc newArc;
		Node pnode;
		if (ps[i].isCenter){
			//// add arc c->t , cost 0, flowcap cap
			pnode = nodesGraph[ps[i].graphNodeIndex];
			newArc = g.addArc(pnode,nodesGraph[indexT]);
			capacity[newArc] = cap;
			cost[newArc] = 0;
			arcCounter++;
			//arcNames[arcCounter] = "c->t";//string("c: node") + std::to_string(i) + "-> t";
		}else{
			//// add arc s->p , cost 0, flowcap 1
			pnode = nodesGraph[ps[i].graphNodeIndex];
			newArc = g.addArc(nodesGraph[indexS],pnode);
			capacity[newArc] = 1;
			cost[newArc] = 0;
			arcCounter++;
			//arcNames[arcCounter] = "s->p";//string("S -> node") + i;
			//forall c add p->c cost dist, flowcap 1
			for(int j=0;j<k;j++){
				Node cnode = nodesGraph[ps[cPoints[j]].graphNodeIndex];
				newArc = g.addArc(pnode, cnode);
				if(beVerbose)cout << g.id(g.source(newArc)) << "->" << g.id(g.target(newArc))<< endl;
				capacity[newArc] = 1;
				double costLocal= ps[i].distanceTo(ps[cPoints[j]]);
				cout<< "ArcCost:"<< costLocal<< endl;
				cost[newArc] = costLocal;
				arcCounter++;
				//string tempS = "p->c";string("p: node") + i +"-> c: node "+ j +" cost:" + ps[i].distanceTo(cPoints[j]);
				//arcNames[arcCounter] ="p->c";;
			}
		}
	}
	if(beVerbose||lilVerbose)printf("Input graph created with %d nodes and %d arcs\n\n",
			countNodes(g), countArcs(g));
	// Initialize NetworkSimplex algorithm object
	//NetworkSimplex<SmartDigraph> ns(g);
	CapacityScaling<SmartDigraph,int,double> ns(g);

	//cout << "In Calc before run" << endl;
	//printDPs(ps, noDP);

	ns.upperMap(capacity).costMap(cost).supplyMap(supply);
	// Run NetworkSimplex
	ns.run();
	// Print total flow cost
	if(beVerbose||lilVerbose)printf("Total flow cost: %f\n\n", ns.totalCost<double>());

	std::cout << "Total cost: " << ns.totalCost<double>() << std::endl;

	// Print flow values on the arcs with ArcIterator
	if(beVerbose)printf("Flow values on arcs:\n");
	for (ArcIt a(g); a != INVALID; ++a) {
		//if it is not s or t
		if(ns.flow(a)>0){
			if(g.id(g.source(a))<noDP){
				if(g.id(g.target(a))<noDP){
					if(beVerbose)printf("Region Map %d->%d\n",g.id(g.source(a))+1 ,g.id(g.target(a))+1);
					regions[g.id(g.source(a))] = g.id(g.target(a));
				}else{
					if(beVerbose)printf("Region Map %d-is Center\n",g.id(g.source(a))+1);
					regions[g.id(g.source(a))] = g.id(g.source(a));
				}
			}
		}
		if(beVerbose)printf("Arc %d: %s->%s:  %d/%d cost: %lf\n", g.id(a),
				vertNoToString(g.id(g.source(a)),noDP).c_str(),
				vertNoToString(g.id(g.target(a)),noDP).c_str(),
				ns.flow(a), capacity[a] ,cost[a]);
	}
	for(int i=0;i<noDP;i++){
		//printf("Region Map %d->%d\n");
		if(beVerbose)printf("index %d value %d\n",i+1,regions[i]+1);
	}

	//cout << "In Calc return " << endl;
	//printDPs(ps, noDP);
	if(beVerbose||lilVerbose)printf("*****calcEnd********\n");
	return ns.totalCost<double>();
}


double bruteForceSearch(DataPoint ps[], int noDP, int k, int cap){
	//start with random centers
	int cPoints[k];
	int cPointsNew[k];
	int regions[noDP];
	int regionsNew[noDP];

	bool beVerbose = true;

    int indArray[noDP];
    for(int j=0;j<noDP;j++) indArray[j]=j;

    std::list<int> lt;
    std::list<std::list<int>> all;
    subset(indArray,noDP,k,0,lt,all);
    cout << "ALL--------------" << endl;
    printList(all);

    double currentCost = DBL_MAX;
    double newCost;

    int pairNo = 0;

    for(list<list<int>>::iterator it=all.begin(); it!=all.end() ; ++it){
    	 std::list<int> l = (*it) ;
    	 int cc = 0;
    	 pairNo++;
    	 // reset centers
    	 for(int j=0;j<noDP;j++){
 			ps[j].isCenter = false;
 			ps[j].centerID = -1;
    	 }

    	 // set centers
        for(list<int>::iterator it=l.begin(); it!=l.end() ; ++it){
        	cout << " " << *it;
			ps[*it].isCenter = true;
			ps[*it].centerID = cc;
			cPoints[cc] = *it;
			cc++;
        }
        newCost = calcRegionsCS(ps, cPoints, noDP, k,cap,regions);

    	if(newCost<currentCost){
    		currentCost = newCost;

    		/*
    		std::ostringstream stringStream;
    		stringStream << "pairNo: "  << pairNo <<" with cost:"<<newCost;
    		std::string formatedStr = stringStream.str();
    		const char* cstr = formatedStr.c_str();
    		directPlotRegions(ps, noDP,cPoints, k,regions,cstr);
    		*/
    		//save centerIndices and region
    	}
    }
	 // reset centers
	 for(int j=0;j<noDP;j++){
		ps[j].isCenter = false;
		ps[j].centerID = -1;
	 }

	cout <<  "BF exit with min Cost:" << currentCost<< endl;;
	return currentCost;
}


//plot functions ************************
void initPlotForFileLink(FILE * gnuplotPipe,const char * title){

	//int ColorCode[noDP];
	int noCMDs = 12;
	const char * commandsForGnuplot[] = {
			"set xrange [0:10]",
			"set yrange [0:10]",
			"set style line 1 lc rgb 'black' pt 5 ps 1  # square",
			"set style line 2 lc rgb 'green' pt 7 ps 1  # circle",
			"set style line 3 lc rgb 'blue' pt 9   # triangle",
			"set style line 4 lc rgb 'black' pt 1   # plus sign",
			"set style line 5 lc rgb 'green' pt 2   # cross",
			"set style line 6 lc rgb 'black' pt 10   # triangle down",
			"set style line 7 lc rgb 'blue' pt 16   #star",
			"set style line 8 lc rgb 'red' pt 16   #star",
			"set style line 9 lc rgb 'red' pt 9   # triangle",
			"set style line 11 lc rgb 'red' pt 7 ps 2  # circle for center"
	};
	printf("set title %s \n", title);
	fprintf(gnuplotPipe, "set title \"%s\" \n", title);
	for (int j=0; j < noCMDs;j++)
	{
		fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[j]); //Send commands to gnuplot
	}
}

void directPlot(DataPoint* dps, int noDP)
{
	FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
	initPlotForFileLink(gnuplotPipe,"directPlot");
	fprintf(gnuplotPipe, "plot '-' \n");
	int i;
	for (i=0; i < noDP; i++)
	{
		fprintf(gnuplotPipe, "%lf %lf\n", dps[i].X, dps[i].Y);
	}
	fprintf(gnuplotPipe, "e");
	fflush(gnuplotPipe);
	pclose(gnuplotPipe);
}

void directPlotPoints(DataPoint* dps, int noDP,int centers[], int noCenters, const char * title)
{
	FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
	initPlotForFileLink(gnuplotPipe,title);
	fprintf(gnuplotPipe, "plot '-' w p ls 11 t  \"Center\" , '-' w p ls 1 t \"DataPoints\" \n");
	int i;
	for (i=0; i < noCenters; i++)
	{
		fprintf(gnuplotPipe, "%lf %lf\n", dps[centers[i]].X, dps[centers[i]].Y);
	}
	fprintf(gnuplotPipe, "e\n");
	int j;
	for (j=0; j < noDP; j++)
	{
		fprintf(gnuplotPipe, "%lf %lf\n", dps[j].X, dps[j].Y);
	}
	fprintf(gnuplotPipe, "e\n");
	fflush(gnuplotPipe);
	pclose(gnuplotPipe);
}

void directPlotRegions(DataPoint* dps, int noDP,int centers[], int noCenters,int regions[],const char * title)
{
	FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
	initPlotForFileLink(gnuplotPipe,title);
	//set multiplot
	fprintf(gnuplotPipe, "set multiplot\n");
	fprintf(gnuplotPipe, "plot '-' w p ls 11 t  \"Center\" \n" ); //, '-' w p ls 1 t \"DataPoints\" \n");
	int i;
	for (i=0; i < noCenters; i++)
	{
		//printf("plotting center point: %lf %lf\n", centers[i].X, centers[i].Y);
		fprintf(gnuplotPipe, "%lf %lf\n", dps[centers[i]].X, dps[centers[i]].Y);
	}
	fprintf(gnuplotPipe, "e\n");
	int j;
	for (j=0; j < noDP; j++)
	{
		fprintf(gnuplotPipe, "plot '-' w p ls %d t \"DataPoints %d \" \n",regions[j]%9+1,regions[j]%9+1);
		fprintf(gnuplotPipe, "%lf %lf\n", dps[j].X, dps[j].Y);
		fprintf(gnuplotPipe, "e\n");
	}
	fflush(gnuplotPipe);
	fprintf(gnuplotPipe, "unset multiplot\n");
	pclose(gnuplotPipe);
}

//end of plot functions ************************

//helper functions *******************************
std::string my_to_string(int Number){
	string res;
	ostringstream oss;
	oss << Number;
	res = oss.str();
	return res;
}

std::string vertNoToString(int vertexNo,int maxNo){

	std::string retString ="";
	if(vertexNo<maxNo){
		retString = my_to_string(vertexNo+1);
	}else{
		if(vertexNo==maxNo) retString = "S";
		else retString = "T";
	}
	return retString;
}

//subset helper

void printList( list<int> l){
    for(list<int>::iterator it=l.begin(); it!=l.end() ; ++it)
            cout << " " << *it;
    cout<<endl;
}

void printList( list<list<int>> l){
    for(list<list<int>>::iterator it=l.begin(); it!=l.end() ; ++it)
         printList(*it) ;
}

void printVector( std::vector<int> v){
    for(std::vector<int>::iterator it=v.begin(); it!=v.end() ; ++it)
            cout << " " << *it;
    cout<<endl;
}

void printVector( std::vector< std::vector<int> > v){
    for(std::vector<std::vector<int>>::iterator it=v.begin(); it!=v.end() ; ++it)
         printVector(*it) ;
}

void subset(int arr[], int size, int left, int index, list<int> &l, list<list<int>> &all){

	if(left==0){
        //printList(l);
        all.push_back(l);
        return;
    }
    for(int i=index; i<size;i++){
    	l.push_back(arr[i]);
        subset(arr,size,left-1,i+1,l, all);
        l.pop_back();
    }

}

//end of helper functions *******************************
