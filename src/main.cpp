#include <iostream>

#include <cstdio>
#include <fstream>

//#include <stdlib>
//#include <stdio>

#include <sys/times.h>
#include <sys/vtimes.h>

#include <string>
#include <cstring>

#include <stdio.h>
#include <list>
#include <float.h>

#include <lemon/maps.h>
#include <lemon/smart_graph.h>
#include <lemon/list_graph.h>
#include <lemon/capacity_scaling.h>
#include <lemon/cost_scaling.h>
#include <lemon/network_simplex.h>
#include <lemon/preflow.h>
#include <lemon/lgf_reader.h>
#include <lemon/lgf_writer.h>
#include <lemon/dimacs.h>

#include "filereader.hpp"
#include "DataPoint.h"
#include "SolverKMedian.h"


int example1();
int example2();
//int example3();
void exampleSearch();
void exampleSearchBF();
void simpleExampleSearchBF();
void mediumExampleSearchBF();
void exampleLoopWithPlot();
void generateRead();
double localSearch(DataPoint ps[], int noDP, int k, int cap,int regionsBest[],int cPointsBest[],bool print=false,bool drawStart=false);
double bruteForceSearch(DataPoint ps[], int noDP, int k, int cap,int regionsBest[],int cPointsBest[]);
void directPlot(DataPoint* dps, int noDP);
void directPlotRegions(DataPoint* dps, int noDP,int centers[], int noCenters,int regions[],const char * title);
void directPlotPoints(DataPoint* dps, int noDP,int centers[], int noCenters,const char * title);
double calcRegionsOLD(DataPoint ps[],int cPoints[], int noDP, int k, int cap, int (regions)[]);
//template <unsigned int SIZE>double calcRegionsCS(DataPoint ps[],int cPoints[], int noDP, int k, int cap, int (&regions)[SIZE]);
double calcRegionsCS(DataPoint ps[],int cPoints[], int noDP, int k, int cap, int* regions);
//double calcRegionsCSRG(DataPoint ps[],int cPointsInds[], int noDP, int k, int cap, int* regions);
//double calcRegionsCSRG(const SmartDigraph& g,DataPoint ps[],int cPointsInds[], int noDP, int k, int cap, int* regions);
double calcRegionsCSRG(const SmartDigraph& g,
		IntArcMap& capacity,
		DoubleArcMap& cost,
		IntNodeMap& supply,
		IntArcMap& name,
		const Node* nodesGraph,
		const Node* centerNodes,
		int indexT,
		int kToBeOutSwitched,
		int iToBeInSwitched,
		DataPoint ps[],int cPointsInds[], int noDP, int k, int cap, int* regions);



double calcRegionsCSRGTest(const SmartDigraph& g,
		IntArcMap& capacity,
		DoubleArcMap& cost,
		IntNodeMap& supply,
		IntArcMap& name,
		const Node* nodesGraph,
		const Node* centerNodes,
		int indexT,
		int kToBeOutSwitched,
		int iToBeInSwitched,
		DataPoint ps[],int cPointsInds[], int noDP, int k, int cap, int* regions);

double localSearchOpt(DataPoint ps[], int noDP, int k, int cap,int* regionsBest,int* cPointsBest, bool printEachImprovement, bool drawStart,int algType);

double calcRegionsNS2(const SmartDigraph& g,
		IntArcMap& capacity,
		DoubleArcMap& cost,
		IntNodeMap& supply,
		IntArcMap& name,
		const Node* nodesGraph,
		const Node* centerNodes,
		int indexT,
		int kToBeOutSwitched,
		int iToBeInSwitched,
		DataPoint ps[],int cPointsInds[], int noDP, int k, int cap, int* regions);

double calcRegionsCostOp(const SmartDigraph& g,
		IntArcMap& capacity,
		DoubleArcMap& cost,
		IntNodeMap& supply,
		IntArcMap& name,
		const Node* nodesGraph,
		const Node* centerNodes,
		int indexT,
		int kToBeOutSwitched,
		int iToBeInSwitched,
		DataPoint ps[],int cPointsInds[], int noDP, int k, int cap, int* regions);

std::string vertNoToString(int vertexNo,int maxNo);
void printList( std::list<int> l);
void printList( std::list<std::list<int>> l);
void printVector( std::vector< std::vector<int> > v);
void printDPs(DataPoint ps[], int noDP);
void subset(int arr[], int size, int left, int index, std::list<int> &l, std::list<std::list<int>> &all);
void updateDPS(DataPoint ps[],int noDP,int center[],int k, int regions[]);
double calculateCost(DataPoint ps[],int noDP,int center[]);
//void generateNodesInGraph(DataPoint ps[],Node* nodesGraph,Node* centerNodes,int noDP, int k,IntNodeMap supply);
void printDPs(DataPoint ps[], int noDP);

using namespace lemon;
using namespace std;
DIGRAPH_TYPEDEFS(SmartDigraph);

int plotmin = 0;
int plotmax = 10;


/*
static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;

void init(){
	FILE* file = fopen("/proc/stat", "r");
	fscanf(file, "cpu %llu %llu %llu %llu",&lastTotalUser,&lastTotalUserLow,&lastTotalSys,&lastTotalIdle);
	fclose(file);
}*/

static clock_t lastCPU,lastSysCPU, lastUserCPU;
static int numProcessor;

void init(){
	FILE * file;
	struct tms timeSample;
	char line[128];
	lastCPU = times(&timeSample);
	lastSysCPU = timeSample.tms_stime;
	lastUserCPU = timeSample.tms_utime;
	file = fopen("/proc/cpuinfo","r");
	numProcessor = 0;
	while(fgets(line,128, file)!= NULL){
		if(strncmp(line,"processor",9)==0)numProcessor++;
	}
fclose(file);
}

double getCurrentValue(){
	struct tms timeSample;
	clock_t now;
	double percent;

	now = times(&timeSample);
	if(now<= lastCPU || timeSample.tms_stime < lastSysCPU||
			timeSample.tms_utime<lastUserCPU){
		//overflow detection
		percent = -1.0;

	}else{
		percent = (timeSample.tms_stime - lastSysCPU) + (timeSample.tms_utime-lastUserCPU);
		percent /= (now-lastCPU);
		percent /= numProcessor;
		percent *=100;
	}
	lastCPU = now;
	lastSysCPU = timeSample.tms_stime;
	lastUserCPU = timeSample.tms_utime;

	return percent;
}

double getCurrentCPUDiff(){
	struct tms timeSample;
	clock_t now;
	double percent;

	now = times(&timeSample);
	if(now<= lastCPU || timeSample.tms_stime < lastSysCPU||
			timeSample.tms_utime<lastUserCPU){
		//overflow detection
		percent = -1.0;

	}else{
		percent = (timeSample.tms_stime - lastSysCPU) + (timeSample.tms_utime-lastUserCPU);
	}
	lastCPU = now;
	lastSysCPU = timeSample.tms_stime;
	lastUserCPU = timeSample.tms_utime;

	return percent;
}
/*
double getCurrentCPUDiff(){
	struct tms timeSample;
	clock_t now;
	double percent;

	now = times(&timeSample);
	if(now<= lastCPU || timeSample.tms_stime < lastSysCPU||
			timeSample.tms_utime<lastUserCPU){
		//overflow detection
		percent = -1.0;

	}else{
		percent = (timeSample.tms_stime - lastSysCPU) + (timeSample.tms_utime-lastUserCPU);
	}
	lastCPU = now;
	lastSysCPU = timeSample.tms_stime;
	lastUserCPU = timeSample.tms_utime;

	return percent;
}
*/

int createDPSFromInput(Data dataBucket,DataPoint dps[]){
	for(std::size_t i=0; i < dataBucket.size(); ++i){
		//for(std::size_t j=0; j < dataBucket[i].size(); ++j)
			//dataBucket[i][j];
		dps[i] = DataPoint(dataBucket[i][0],dataBucket[i][1]);
	}
	return dataBucket.size();
}

int createDPSFromInput(DataDouble dataBucket,DataPoint dps[]){
	for(std::size_t i=0; i < dataBucket.size(); ++i){
		//for(std::size_t j=0; j < dataBucket[i].size(); ++j)
			//dataBucket[i][j];
		dps[i] = DataPoint(dataBucket[i][0],dataBucket[i][1]);
	}
	return dataBucket.size();
}

int createDataFileTest(int noPoints,int cluster, std::string filename){

	std::string fname = string("./res/") + filename;
	std::ofstream out(fname);

    for(int i=0;i<noPoints;++i){
    	//std::string input = string("1,");
    	out << "1," << i;
    	out << endl;
    }
	out.close();
    return 0;

}

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

int createDataFile(int noPoints,int cluster,double clusterDist,double inClusterDist, std::string filename){

	std::string fname = string("./res/") + filename;
	std::ofstream out(fname);

	srand (time(NULL));
	//seed:1526051006
	//srand (1526051006);

	double maxIn = cluster*1.1*clusterDist+2*inClusterDist;

	int pPerC = noPoints/cluster;
	double clusterCenters[cluster];
	for(int i=0;i<cluster;i++){
		clusterCenters[i] = i*clusterDist+ fRand(0,(clusterDist*0.1))-(clusterDist*0.1)/2.0 ;
		out << clusterCenters[i] << ",1.0";
    	out << endl;
		//i*clusterDist+ frand(0,(clusterDist*0.1));
	}

    for(int i=0;i<(noPoints-cluster);i++){
    	//std::string input = string("1,");
    	out << clusterCenters[(int)floor(i/pPerC)] + fRand(0,(inClusterDist))- (inClusterDist)/2 << ",1.0";
    	out << endl;
    }
	out.close();
    return 0;
}

void generateReadTest(){
	Data dataBucket;
	int extremes[2];
	createDataFileTest(12,1, "2d-test-gen");
	readIntsFromFile("./res/2d-test-gen",dataBucket,extremes);
	plotmin=extremes[0];
	plotmax=extremes[1];
	DataPoint readDPS [dataBucket.size()];
	int noReadDPS = createDPSFromInput(dataBucket,readDPS);
	printDPs(readDPS,noReadDPS);
	directPlot(readDPS,noReadDPS);
}

void generateRead(){
	Data inputBucket;
	int doncare[2];

	///home/soyo/Development/BA/SecondLemonWorkspace/LKM_V3/res/input-para
	//readIntsFromFile("./res/input-para",inputBucket,doncare);
	readIntsFromFile("/home/soyo/Development/BA/SecondLemonWorkspace/LKM_V3/res/input-para",inputBucket,doncare);


	int bruteforceMax = 30;
	int drawMax = 500;

	DataDouble dataBucket;
	double extremes[2];
	//(int noPoints,int cluster,double clusterDist,double inClusterDist
	//createDataFile(10,2,30,8, "2dd-test-gen");

	createDataFile(inputBucket[0][0],
			inputBucket[0][1],
			inputBucket[0][2],
			inputBucket[0][3], "2dd-test-gen");

	readDoublesFromFile("./res/2dd-test-gen",dataBucket,extremes);
	plotmin=extremes[0];
	plotmax=extremes[1];
	DataPoint readDPS [dataBucket.size()];
	int noReadDPS = createDPSFromInput(dataBucket,readDPS);
	//printDPs(readDPS,noReadDPS);
	//directPlot(readDPS,noReadDPS);
	int centers[2] ={1,2};
	//if(noReadDPS<drawMax)directPlotPoints(readDPS, noReadDPS, centers,2, "PlotWithConstructedCluster");

	int k = inputBucket[0][1];
	int cap = inputBucket[0][4];

	int algType = inputBucket[0][5];

	int bestC[k];
	int bestRegs[noReadDPS];

	string plotName = string("not set");
	std::ostringstream strs;


	if(noReadDPS<bruteforceMax){
		double bCost = bruteForceSearch(readDPS, noReadDPS,  k,  cap,bestRegs, bestC);

		updateDPS(readDPS,noReadDPS,bestC,k,bestRegs);
		cout <<"---- after bfSeach:\n";
		//printDPs(readDPS,noReadDPS);
		double bcalcCost = calculateCost(readDPS,noReadDPS,bestC);

		for(int z=0;z<k;z++){
			cout << "bf center:"<< bestC[z]<< endl;
		}
		for(int z=0;z<noReadDPS;z++){
			cout << "bf regions:"<< bestRegs[z]<< endl;
		}

		cout << "bCost:" << bCost << " bcalcCost:" << bcalcCost<<  endl;


		strs << bCost;
		std::string str = strs.str();
		string plotName = string("bruteForceSeach Cost:") + str;
		directPlotRegions(readDPS, noReadDPS,bestC, k,bestRegs,plotName.c_str());

//		cout << "Press any key to continue" << endl;
//		std::cin.get();
	}

	cout << "Press any key to continue" << endl;
		std::cin.get();

	int bestC2[k];
	int bestRegs2[noReadDPS];



	struct tms timeSample;
	clock_t now;
	double startProcTime;
	double endProcTime;
	//now = times(&timeSample);
	//startProcTime = timeSample.tms_stime+timeSample.tms_utime;

	//double lsCost =0;

	double lsCost = localSearchOpt(readDPS, noReadDPS,  k,  cap, bestRegs2, bestC2,false,false,algType);//noReadDPS<drawMax);
	//now = times(&timeSample);

	//endProcTime = timeSample.tms_stime+timeSample.tms_utime;

	//cout << "ProcessTime:" << (endProcTime-startProcTime) << "Press any key to continue" << endl;
	//std::cin.get();

	/*
	now = times(&timeSample);
	startProcTime = timeSample.tms_stime+timeSample.tms_utime;
	double lsCostOLD = localSearch(readDPS, noReadDPS,  k,  cap, bestRegs2, bestC2,false,noReadDPS<drawMax);

	now = times(&timeSample);
	endProcTime = timeSample.tms_stime+timeSample.tms_utime;

	cout << "ProcessTimeOld:" << (endProcTime-startProcTime) << "Press any key to continue" << endl;
	std::cin.get();
	 */

	updateDPS(readDPS,noReadDPS,bestC2,k,bestRegs2);
	cout <<"---- after localSeach:\n";
	//printDPs(readDPS,noReadDPS);


	/*
	for(int z=0;z<k;z++){
		cout << "ls center:"<< bestC2[z]<< endl;
	}
	if(noReadDPS<600)
	for(int z=0;z<noReadDPS;z++){
		cout << "ls regions:"<< bestRegs2[z]<< endl;
	}
	 */

	//double lcalcCost = calculateCost(readDPS,noReadDPS,bestC2);

	cout << "lsCost:" << lsCost <<  endl;

	std::ostringstream strs2;
	strs2 << lsCost;
	std::string str2 = strs2.str();
	plotName = string("localSeach Cost:") + str2;
	cout << "plotName:"<<plotName<<endl;
	//cout << "oldLSCost:" <<lsCostOLD << endl;
	if(noReadDPS<drawMax)directPlotRegions(readDPS, noReadDPS,bestC2, k,bestRegs2,plotName.c_str());

}

void readExample(){
	Data dataBucket;
		int extremes[2];
		//readIntsFromFile("filereader-test",dataBucket);//char del optional;
		readIntsFromFile("./res/2d-test",dataBucket,extremes);
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

		cout<< "Max: "<< extremes[1] << " Min: " << extremes[0] << endl;

		plotmin=extremes[0];
		plotmax=extremes[1];

		DataPoint readDPS [dataBucket.size()];
		int noReadDPS = createDPSFromInput(dataBucket,readDPS);

		printDPs(readDPS,noReadDPS);
		directPlot(readDPS,noReadDPS);

}

void readDoubleExample(){
	DataDouble dataBucket;
	double extremes[2];
	readDoublesFromFile("./res/2DD-test",dataBucket,extremes);


	cout << "main" << endl;
	//printVector(dataBucket);
	cout << "after pv" << endl;
	// Output to see if everything was read correctly
	for(std::size_t i=0; i < dataBucket.size(); ++i){
		for(std::size_t j=0; j < dataBucket[i].size(); ++j){
			std::cout << std::setw(5) << dataBucket[i][j] << " ";
		}
		std::cout << std::endl;
	}

	cout<< "Max: "<< extremes[1] << " Min: " << extremes[0] << endl;

	plotmin=extremes[0];
	plotmax=extremes[1];

	DataPoint readDPS [dataBucket.size()];
	int noReadDPS = createDPSFromInput(dataBucket,readDPS);

	printDPs(readDPS,noReadDPS);
	directPlot(readDPS,noReadDPS);

}

//main
int main() {

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


	//readDoubleExample();
	//readExample();
	//mediumExampleSearchBF();

	//last good
	//exampleSearch();
	generateRead();
	//example2();
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
	bruteForceSearch(ps, noDP,  k,  cap, NULL,NULL);

}

void simpleExampleSearchBF(){

	int noDP = 3;
	int k = 1;
	int cap = 2;
	DataPoint p1(1,1);
	DataPoint p2(2,1);
	DataPoint p3(3,1);
	DataPoint ps[noDP] = {p1,p2,p3};

	bruteForceSearch(ps, noDP,  k,  cap, NULL,NULL);
}

void mediumExampleSearchBF(){

	int noDP = 6;
	int k = 2;
	int cap = 3;
	DataPoint p1(1,1);
	DataPoint p2(2,1);
	DataPoint p3(3,1);

	DataPoint p4(1,3);
	DataPoint p5(2,3);
	DataPoint p6(3,3);


	DataPoint ps[noDP] = {p1,p2,p3,p4,p5,p6};

	int bestC[k];
	int bestRegs[noDP];

	bruteForceSearch(ps, noDP,  k,  cap,bestRegs, bestC);

	updateDPS(ps,noDP,bestC,k,bestRegs);

	directPlotRegions(ps, noDP,bestC, k,bestRegs,"bruteForceSeach");

	localSearch(ps, noDP,  k,  cap, bestRegs, bestC);

	updateDPS(ps,noDP,bestC,k,bestRegs);

	directPlotRegions(ps, noDP,bestC, k,bestRegs,"localSeach");


}

void exampleSearch(){
	int noDP = 8;
	int k = 3;
	int cap = 3;
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
	int bestC[k];
	int bestRegs[noDP];
	localSearch(ps, noDP,  k,  cap, bestRegs, bestC);
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
	double newCost = calcRegionsOLD(ps,cPoints, noDP, k,cap,regions);
	directPlotPoints(ps, noDP,cPoints, k, "Example");
	directPlotRegions(ps, noDP,cPoints, k,regions,"Example");
}


void printVector( DataVector v){
    for(DataVector::iterator it=v.begin(); it!=v.end() ; ++it)
            //cout << " " << *it;
    		it->print();
    cout<<endl;
}

class MyClass {
    vector<int>& vec;

public:
    // this is the right way to assign
    // the reference of stl container
    MyClass(vector<int>& arr)
        : vec(arr)
    {
    }
    void print()
    {
    	cout <<"start o\n";
        /// print the value of vector
        for (int i = 0; i < vec.size(); i++)
            cout << vec[i] << " ";
        cout <<"end o\n";
    }
};

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

    DataPoint ps2[2] = {p1,p2};

    cout <<"out: ps&: " << &ps << endl;

    printDPs(ps2,2);

    DataVector v;
    v.push_back(p1);
    v.push_back(p2);

    cout <<"out: v&: " << &v << endl;
    SolverKMedian skm(v,2,1,1);

    skm.printContent();

    cout << "out add" << endl;
    v.pop_back();
    v.push_back(p8);
    printVector(v);
    skm.printContent();

    vector<int> vec;
    for (int i = 1; i <= 5; i++)
        vec.push_back(i);
    MyClass obj(vec);
    obj.print();
    vec.push_back(19);
    vec.push_back(21);
    obj.print();

    //return calcRegions(ps,cPoints, noDP, k,cap,regions);
}

int example2(){
	int noDP = 8;
	int k = 3;
	int cap = 3;
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
	int bestC[k];
	int bestRegs[noDP];
	//localSearchOpt(ps, noDP,  k,  cap, bestRegs, bestC,false,true);



	bruteForceSearch(ps, noDP,  k,  cap,bestRegs, bestC);

	updateDPS(ps,noDP,bestC,k,bestRegs);

	directPlotRegions(ps, noDP,bestC, k,bestRegs,"bruteForceSeach");

	localSearchOpt(ps, noDP,  k,  cap, bestRegs, bestC,false,true,1);
	//localSearch(ps, noDP,  k,  cap, bestRegs, bestC);

	updateDPS(ps,noDP,bestC,k,bestRegs);

	directPlotRegions(ps, noDP,bestC, k,bestRegs,"localSeach");



}
/*
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
		cout << "ps[k].X: " << ps[k].X << endl;
		cout << "ps[k].Y: " << ps[k].Y << endl;
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
double localSearch(DataPoint ps[], int noDP, int k, int cap,int* regionsBest,int* cPointsBest, bool printEachImprovement, bool drawStart){

	bool beVerbose = false;
	//start with random centers
	int cPoints[k];
	int cPointsNew[k];
	int regions[noDP];
	//int regionsBest[noDP];
	//int cPointsBest[k];

	srand (time(NULL));
	//seed:1526051006
	srand (1526051006);

	//make sure no centers are set
	 for(int j=0;j<noDP;j++){
		ps[j].isCenter = false;
		ps[j].centerID = -1;
	 }

	if(beVerbose)cout << "srand seed:" << time(NULL) << endl;
	for(int i=0;i<k;i++){
		bool notFound = true;
		while(notFound){
			int randVal = rand();
			if(beVerbose)cout <<"rnd:"<< randVal <<endl;
			int randInt = randVal % noDP;
			if(beVerbose)cout <<"randInt:"<< randInt <<endl;

			if(!ps[randInt].isCenter){
				ps[randInt].isCenter = true;
				ps[randInt].centerID = i;
				cPoints[i] = randInt;
				cPointsNew[i] = randInt;
				if(beVerbose)cout << "cPoints[i]: x " << ps[cPoints[i]].X << " y:"<< ps[cPoints[i]].Y << endl;
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
	if(beVerbose)printf("currentCost: %f",currentCost);

	std::ostringstream stringStream;
	stringStream << "Start with cost:" << currentCost;
	//stringf();
	std::string formatedStr = stringStream.str();
	const char* cstr = formatedStr.c_str();

	//directPlot(ps, noDP);
	//directPlotPoints(ps, noDP,cPoints, k, "Start");
	if(drawStart)directPlotRegions(ps, noDP,cPoints, k,regions,cstr);

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
					ps[cPoints[j]].centerID = -1;
					ps[l].isCenter = true;
					ps[l].centerID = ps[cPoints[j]].centerID;
					//ps[cPoints[j]].centerID = -1;
					if(beVerbose)cout <<"---after try swap --\n";
					//cout << "ps[l] isCenter expected 1 :" << ps[l].isCenter << endl;
					//cout << "ps[cPoints[j]].isCenter expected 0:" << ps[cPoints[j]].isCenter << endl;
					//cout << "ps[cPoints[j].graphNodeIndex].isCenter expected 0:" << ps[ps[cPoints[j]].graphNodeIndex].isCenter << endl;
					//cout <<"---end try--\n";

					if(beVerbose)cout << "After switch" << endl;
					if(beVerbose)cout << "In LS before iter calc" << endl;

					//cout << "calc start with tree creation here:"<< getCurrentCPUDiff() << endl;
					newCost = calcRegionsCS(ps, cPointsNew, noDP, k,cap,regions);
					//cout << "calc end with tree creation here:"<< getCurrentCPUDiff() << endl;

					//cout << "In LS after iter calc" << endl;
					//printDPs(ps, noDP);

					cout << "newCost:" << newCost << endl;
					if(beVerbose)cout << "currentCost:" << currentCost << endl;

					if(newCost<currentCost){
						if(beVerbose)cout <<"found better solution at j:" << j <<" l:" << l << endl;
						if(beVerbose)cout << "switched: " << cPoints[j] << "with " << l << endl;

						cPoints[j] = cPointsNew[j];
						improvement = currentCost-newCost;
						currentCost=newCost;

						std::ostringstream stringStream;
						stringStream << "Iteration: "  << iterations <<" with cost:"<<currentCost;
						std::string formatedStr = stringStream.str();
						const char* cstr = formatedStr.c_str();
						if(printEachImprovement)directPlotRegions(ps, noDP,cPoints, k,regions,cstr);

						if(cPointsBest!=NULL && regionsBest!=NULL){
							for(int p=0;p<k;p++){
								cPointsBest[p]=cPoints[p];
							}
							for(int v=0;v<noDP;v++){
								regionsBest[v]=regions[v];
							}
						}
						forBreak = true;
						break;
						//goto stop;
					}else{
						ps[cPoints[j]].isCenter = true;
						ps[cPoints[j]].centerID = ps[cPointsNew[j]].centerID;
						ps[cPointsNew[j]].isCenter = false;
						ps[cPointsNew[j]].centerID = -1;
						if(beVerbose)cout <<"++++swap back+++\n";
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
		if(beVerbose)cout << "After iteration:" << iterations << endl;
		if(forBreak)forBreak = false;
		else improvement = 0;

	}

	for(int z=0;z<k;z++){
		cout << "in ls  center:"<< cPointsBest[z]<< endl;
	}

	if(beVerbose)cout <<  "While Loop exited at iter:" << iterations <<endl;
	if(beVerbose)cout << "MinCost found:" <<currentCost<<endl;
	return currentCost;
}

double localSearchOpt(DataPoint ps[], int noDP, int k, int cap,int* regionsBest,int* cPointsBest, bool printEachImprovement, bool drawStart,int algType){

	bool beVerbose = true;
	//start with random centers
	int cPoints[k];
	int cPointsNew[k];
	int regions[noDP];
	//int regionsBest[noDP];
	//int cPointsBest[k];

	srand (time(NULL));
	//seed:1526051006
	srand (1526051006);

	//make sure no centers are set
	 for(int j=0;j<noDP;j++){
		ps[j].isCenter = false;
		ps[j].centerID = -1;
	 }

	if(beVerbose)cout << "srand seed:" << time(NULL) << endl;
	for(int i=0;i<k;i++){
		bool notFound = true;
		while(notFound){
			int randVal = rand();
			if(beVerbose)cout <<"rnd:"<< randVal <<endl;
			int randInt = randVal % noDP;
			if(beVerbose)cout <<"randInt:"<< randInt <<endl;

			if(!ps[randInt].isCenter){
				ps[randInt].isCenter = true;
				ps[randInt].centerID = i;
				cPoints[i] = randInt;
				cPointsNew[i] = randInt;
				if(beVerbose)cout << "cPoints[i]: x " << ps[cPoints[i]].X << " y:"<< ps[cPoints[i]].Y << endl;
				notFound = false;
			}
		}
	}

	//create graph
	SmartDigraph g;
	//map for cost and capacity
	IntArcMap capacity(g);
	DoubleArcMap cost(g);
	IntNodeMap supply(g);
	//Digraph::NodeMap<string>
	IntArcMap name(g);
	//StringArcMap name(g);
	Node nodesGraph[noDP+2+k];
	Node centerNodes[k];
	//for all points
	//add vertex

//	int cPoints[k];
	/*
	generateNodesInGraph(g,ps,nodesGraph,cPointsInds,cPoints,
		centerNodes,noDP, k,supply);
	 */

	for(int i=0;i<noDP;i++){
		ps[i].graphNodeIndex = i;
		nodesGraph[i] = g.addNode();
		supply[nodesGraph[i]] = 0;
	}
	// add s and t
	nodesGraph[noDP] = g.addNode();
	nodesGraph[noDP+1] = g.addNode();
	int indexS = noDP;
	int indexT = noDP+1;

	for(int i=0;i<k;i++) {
		//cPoints[i] = cPoints[i];
		centerNodes[i]=g.addNode();
		nodesGraph[indexT+1+i]=centerNodes[i];
	}


	int demand = (noDP);
	if(beVerbose)cout << "demand:" << demand << endl;
	supply[nodesGraph[indexS]] = (demand);
	supply[nodesGraph[indexT]] = -(demand);

	int numberOfArcs = (noDP)*(k+1)+k;
	if(beVerbose)cout << "expected numberOfArcs:" << numberOfArcs << endl;

	int arcCounter = 0;
	Arc newArc;
	Node pnode;
	//for all points
	for(int i=0;i<noDP;i++){
			//// add arc s->p , cost 0, flowcap 1
			pnode = nodesGraph[ps[i].graphNodeIndex];
			newArc = g.addArc(nodesGraph[indexS],pnode);
			capacity[newArc] = 1;
			cost[newArc] = 0;
			arcCounter++;
			//arcNames[arcCounter] = "s->p";//string("S -> node") + i;
			//forall c add p->c cost dist, flowcap 1
			for(int j=0;j<k;j++){
				Node cnode = centerNodes[j];
				newArc = g.addArc(pnode, cnode);
				//if(beVerbose)cout << g.id(g.source(newArc)) << "->" << g.id(g.target(newArc))<< endl;
				capacity[newArc] = 1;
				double costLocal= ps[i].distanceTo(ps[cPoints[j]]);
				//if(beVerbose)ps[i].print();
				//if(beVerbose)ps[cPoints[j]].print();
				//if(beVerbose)cout<< "ArcCost:"<< costLocal<< endl;
				cost[newArc] = costLocal;
				arcCounter++;
				//string tempS = "p->c";string("p: node") + i +"-> c: node "+ j +" cost:" + ps[i].distanceTo(cPoints[j]);
				//arcNames[arcCounter] ="p->c";;
			}
	} //end for all point nodes
	for(int j=0;j<k;j++){
		//// add arc c->t , cost 0, flowcap cap
		pnode = centerNodes[j];
		newArc = g.addArc(pnode,nodesGraph[indexT]);
		capacity[newArc] = cap;
		cost[newArc] = 0;
		arcCounter++;
		//arcNames[arcCounter] = "c->t";//string("c: node") + std::to_string(i) + "-> t";
	}//for all center nodes

	if(beVerbose)printf("Input graph created with %d nodes and %d arcs\n\n",
			countNodes(g), countArcs(g));

	printf("Input graph created with %d nodes and %d arcs\n\n",
				countNodes(g), countArcs(g));

	//// graph created

	//double currentCost = calcRegionsCSRG(g,ps, cPoints, noDP, k,cap,regions);

	/*
	calcRegionsCSRGTest( g,
			capacity,
			cost,
			supply,
			name,
			nodesGraph,
			centerNodes,
			indexT,
			0,//int kToBeOutSwitched,
			0,//int iToBeInSwitched,
			ps,cPoints,noDP,k, cap, regions);
*/

/*
	cout <<"after Test:-----------------" << endl;

	for (ArcIt a(g); a != INVALID; ++a) {
		//cout << "g.id(g.target(a)):" << g.id(g.target(a)) << endl;

		printf("Arc %d: %d->%d:  cap %d cost: %lf\n", g.id(a),
						g.id(g.source(a)),
						g.id(g.target(a)),
						capacity[a] ,cost[a]);
	}
*/
	//calcRegionsCSRGTest( g,
	double (*calcFunc)(const SmartDigraph&,
			IntArcMap&,
			DoubleArcMap&,
			IntNodeMap&,
			IntArcMap&,
			const Node*,
			const Node*,
			int,
			int,
			int,
			DataPoint[],int[], int, int, int , int*);

	switch (algType) {
		case 1:
			calcFunc = calcRegionsNS2;
			break;
		case 2:
			calcFunc = calcRegionsCSRGTest;
			break;
		case 3:
			calcFunc = calcRegionsCSRG;
			break;
		default:
		case 4:
			calcFunc = calcRegionsCostOp;
			break;
	}

	double currentCost  =	calcRegionsNS2(g,
			capacity,
			cost,
			supply,
			name,
			nodesGraph,
			centerNodes,
			indexT,
			0,//int kToBeOutSwitched,
			0,//int iToBeInSwitched,
			ps,cPoints,noDP,k, cap, regions);

	/*
	double currentCost = calcRegionsCSRG( g,
			capacity,
			cost,
			supply,
			name,
			nodesGraph,
			centerNodes,
			indexT,
			ps,cPoints,noDP,k, cap, regions);


*/

	//cout << "In LS after first calc" << endl;
	//printDPs(ps, noDP);

	double newCost;
	double improvement = currentCost;
	double minImprovment = 0.01;
	if(beVerbose)printf("currentCost: %f",currentCost);

	//return 0;

	std::ostringstream stringStream;
	stringStream << "Start with cost:" << currentCost;
	//stringf();
	std::string formatedStr = stringStream.str();
	const char* cstr = formatedStr.c_str();

	//directPlot(ps, noDP);
	//directPlotPoints(ps, noDP,cPoints, k, "Start");
	if(drawStart)directPlotRegions(ps, noDP,cPoints, k,regions,cstr);

	//cout << "After print" << endl;
	//printDPs(ps, noDP);

	bool forBreak = false;
	int iterations = 0;
	int changeCounter = 0;
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
					ps[cPoints[j]].centerID = -1;
					ps[l].isCenter = true;
					ps[l].centerID = ps[cPoints[j]].centerID;
					//ps[cPoints[j]].centerID = -1;
					if(beVerbose)cout <<"---after try swap --\n";
					//cout << "ps[l] isCenter expected 1 :" << ps[l].isCenter << endl;
					//cout << "ps[cPoints[j]].isCenter expected 0:" << ps[cPoints[j]].isCenter << endl;
					//cout << "ps[cPoints[j].graphNodeIndex].isCenter expected 0:" << ps[ps[cPoints[j]].graphNodeIndex].isCenter << endl;
					//cout <<"---end try--\n";

					if(beVerbose)cout << "After switch" << endl;
					if(beVerbose)cout << "In LS before iter calc" << endl;
					//double newCostOld = calcRegionsCS(ps, cPointsNew, noDP, k,cap,regions);

					changeCounter++;

					bool verboseTime = false;
					if(verboseTime)cout << "changeCounter: " << changeCounter << endl;

					if(verboseTime)cout << "calcReg start until here:"<< getCurrentCPUDiff() << endl;

					//calcRegionsCSRGTest( g,
					newCost  =	calcRegionsNS2( g,
							capacity,
							cost,
							supply,
							name,
							nodesGraph,
							centerNodes,
							indexT,
							0,//int kToBeOutSwitched,
							0,//int iToBeInSwitched,
							ps,cPointsNew,noDP,k, cap, regions);

					if(verboseTime)cout << "calcReg end here:"<< getCurrentCPUDiff() << endl;


					if(verboseTime)cout <<"newCost:"<< newCost << endl;
					//cout <<"newCostOld:"<< newCostOld << endl;

					/*
					if(newCostOld==newCost){
						printf("BINGO!\n");
					}else{
						cout << "ARG Different Costs!!! Press any key to continue" << endl;
											std::cin.get();

					} ;
					 */

					//cout << "In LS after iter calc" << endl;
					//printDPs(ps, noDP);

					//if(beVerbose)
					cout << "currentCost:" << currentCost << endl;

					if(newCost<currentCost){
						if(beVerbose)cout <<"found better solution at j:" << j <<" l:" << l << endl;
						if(beVerbose)cout << "switched: " << cPoints[j] << "with " << l << endl;

						cPoints[j] = cPointsNew[j];
						improvement = currentCost-newCost;
						currentCost=newCost;

						cout << "new Min cost:" << currentCost << endl;

						std::ostringstream stringStream;
						stringStream << "Iteration: "  << iterations <<" with cost:"<<currentCost;
						std::string formatedStr = stringStream.str();
						const char* cstr = formatedStr.c_str();
						if(printEachImprovement)directPlotRegions(ps, noDP,cPoints, k,regions,cstr);

						if(cPointsBest!=NULL && regionsBest!=NULL){
							for(int p=0;p<k;p++){
								cPointsBest[p]=cPoints[p];
							}
							for(int v=0;v<noDP;v++){
								regionsBest[v]=regions[v];
							}
						}
						forBreak = true;
						break;
						//goto stop;
					}else{
						ps[cPoints[j]].isCenter = true;
						ps[cPoints[j]].centerID = ps[cPointsNew[j]].centerID;
						ps[cPointsNew[j]].isCenter = false;
						ps[cPointsNew[j]].centerID = -1;
						if(beVerbose)cout <<"++++swap back+++\n";
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
				break;
			}
		} //for j= current Center to switch
		iterations++;
		if(beVerbose)cout << "After iteration:" << iterations << endl;
		if(forBreak)forBreak = false;
		else improvement = 0;

	}



	if(beVerbose){
		for(int z=0;z<k;z++){
				cout << "in ls  center:"<< cPointsBest[z]<< endl;
			}
		cout <<  "While Loop exited at iter:" << iterations <<endl;

	}if(beVerbose)cout << "MinCost found:" <<currentCost<<endl;
	return currentCost;
}


//returns cost of mapping to given centers, regions is map #index of DP to index in DP of center
double calcRegionsOLD(DataPoint ps[],int cPointsInds[], int noDP, int k, int cap, int (regions)[]){


	bool beVerbose = false;
	bool lilVerbose = false;

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

void recursiveMapping(){

}

//bruteForce calc of Regions:
double calcRegionsBF(DataPoint ps[],int cPointsInds[],int cPoints[], int noDP, int k, int cap, int (regions)[]){
	//all possible
	//is valid? then check and compare
	for(int j=0;j<noDP;j++){

	}


}

void generateNodesInGraph(const SmartDigraph& g ,DataPoint ps[],Node* nodesGraph,int cPointsInds[],
		int cPoints[],Node* centerNodes,int noDP, int k,IntNodeMap supply){

	/*
	//SmartDigraph g = gIn;
	for(int i=0;i<noDP;i++){
		ps[i].graphNodeIndex = i;
		nodesGraph[i] = g.addNode();
		supply[nodesGraph[i]] = 0;
	}
	// add s and t
	nodesGraph[noDP] = g.addNode();
	nodesGraph[noDP+1] = g.addNode();
	int indexS = noDP;
	int indexT = noDP+1;

	for(int i=0;i<k;i++) {
		cPoints[i] = cPointsInds[i];
		centerNodes[i]=g.addNode();
		nodesGraph[indexT+1+i]=centerNodes[i];
	}*/
}


double calcRegionsCSRGTest(const SmartDigraph& g,
		IntArcMap& capacity,
		DoubleArcMap& cost,
		IntNodeMap& supply,
		IntArcMap& name,
		const Node* nodesGraph,
		const Node* centerNodes,
		int indexT,
		int kToBeOutSwitched,
		int iToBeInSwitched,
		DataPoint ps[],int cPointsInds[], int noDP, int k, int cap, int* regions){

	//g.addNode();

	bool beVerboseAboutFlow= false;
	bool beVerbose = false;

	if(beVerbose)printf("Input graph created with %d nodes and %d arcs\n\n",
				countNodes(g), countArcs(g));
	Arc newArc;
	Node pnode;

	if(beVerbose){
	for(int z=0;z<k;z++)
		printf("cPointsInds[%d] = %d",z,cPointsInds[z]);
	}
	for(int i=0;i<noDP;i++){
			//// add arc s->p , cost 0, flowcap 1
			pnode = nodesGraph[ps[i].graphNodeIndex];
			//forall c add p->c cost dist, flowcap 1
			for (OutArcIt e(g, pnode); e!=INVALID; ++e){
				int target = g.id(g.target(e));
				if(target>noDP+1){
					target = target - noDP-2;
					int cInd = cPointsInds[target];
					cost[e] = ps[g.id(g.source(e))].distanceTo(ps[cInd]);
					if(beVerbose)printf("OutArc %d: %d->Center %d with id %d:  cap %d cost: %lf\n", g.id(e),
											g.id(g.source(e)),
											target,cInd,
											capacity[e] ,cost[e]);
				}else{
					if(beVerbose)printf("OutArc %d: %d->%d:  cap %d cost: %lf\n", g.id(e),
									g.id(g.source(e)),
									g.id(g.target(e)),
									capacity[e] ,cost[e]);
				}
			}
			/*
			for(int j=0;j<k;j++){
				Node cnode = centerNodes[j];
				newArc =
				capacity[newArc] = 1;
				double costLocal= ps[i].distanceTo(ps[cPointsInds[j]]);
				cost[newArc] = costLocal;
			}*/
	} //end for all point nodes


	if(beVerbose)for (ArcIt a(g); a != INVALID; ++a) {
		//cout << "g.id(g.target(a)):" << g.id(g.target(a)) << endl;
		printf("End Arc %d: %d->%d:  cap %d cost: %lf\n", g.id(a),
						g.id(g.source(a)),
						g.id(g.target(a)),
						capacity[a] ,cost[a]);
		//cost[a]= 100;
		//capacity[a] = 5;
		//if it is not s or t
		//if(ns.flow(a)>0){
		/*
		if(g.id(g.source(a))<noDP){
				//WRONG!!indexT+1+i == centerIndex => centerID = node_CenterIndex-indexT-1
				int regionID = g.id(g.target(a))-indexT-1;
				regions[g.id(g.source(a))] = regionID;
					cout << "g.id(g.target(a)):" << g.id(g.target(a)) << endl;
					//if(beVerbose)
					printf("CS Region Map %d->%d\n",g.id(g.source(a))+1 ,regionID);
					cout << "g.id(g.source(a)):" << g.id(g.source(a)) << endl;
			}
		//}// end if flow
	*/
	}

	//cout << "capacityScale start until here:"<< getCurrentCPUDiff() << endl;

	//NetworkSimplex<SmartDigraph> ns(g);
	CapacityScaling<SmartDigraph,int,double> ns(g);

	//cout << "capacityScale end here:"<< getCurrentCPUDiff() << endl;

	//cout << "In Calc before run" << endl;
	//printDPs(ps, noDP);

	//cout << "capacityScale supply and run here:"<< getCurrentCPUDiff() << endl;
	ns.upperMap(capacity).costMap(cost).supplyMap(supply);
	//cout << "capacityScale run starts here:"<< getCurrentCPUDiff() << endl;
	// Run NetworkSimplex
	ns.run();
	// Print total flow cost
	//cout << "capacityScale run end here:"<< getCurrentCPUDiff() << endl;

	for (ArcIt a(g); a != INVALID; ++a) {
		//if it is not s or t
		if(ns.flow(a)>0){
			if(g.id(g.source(a))<noDP){
				//WRONG!!indexT+1+i == centerIndex => centerID = node_CenterIndex-indexT-1
				int regionID = g.id(g.target(a))-indexT-2;
				regions[g.id(g.source(a))] = g.id(g.target(a))-indexT-1;;

				if(beVerboseAboutFlow){
					cout << "g.id(g.target(a)):" << g.id(g.target(a)) << endl;
					//if(beVerbose)
					printf("CS Region Map %d->%d\n",g.id(g.source(a))+1 ,regionID);
					cout << "g.id(g.source(a)):" << g.id(g.source(a)) << endl;
				}
			}
		}
	}//end for arcIt a

	for(int i=0;i<noDP;i++){
		//printf("Region Map %d->%d\n");
		if(beVerbose)printf("index %d value %d\n",i+1,regions[i]);
	}

	//cout << "In Calc return " << endl;
	//printDPs(ps, noDP);
	if(beVerbose)printf("*****calcEnd********\n");
	return ns.totalCost<double>();

}

double calcRegionsCostOp(const SmartDigraph& g,
		IntArcMap& capacity,
		DoubleArcMap& cost,
		IntNodeMap& supply,
		IntArcMap& name,
		const Node* nodesGraph,
		const Node* centerNodes,
		int indexT,
		int kToBeOutSwitched,
		int iToBeInSwitched,
		DataPoint ps[],int cPointsInds[], int noDP, int k, int cap, int* regions){

	//g.addNode();

	bool beVerboseAboutFlow= false;
	bool beVerbose = false;

	if(beVerbose)printf("Input graph created with %d nodes and %d arcs\n\n",
				countNodes(g), countArcs(g));
	Arc newArc;
	Node pnode;

	if(beVerbose){
	for(int z=0;z<k;z++)
		printf("cPointsInds[%d] = %d",z,cPointsInds[z]);
	}
	for(int i=0;i<noDP;i++){
			//// add arc s->p , cost 0, flowcap 1
			pnode = nodesGraph[ps[i].graphNodeIndex];
			//forall c add p->c cost dist, flowcap 1
			for (OutArcIt e(g, pnode); e!=INVALID; ++e){
				int target = g.id(g.target(e));
				if(target>noDP+1){
					target = target - noDP-2;
					int cInd = cPointsInds[target];
					cost[e] = ps[g.id(g.source(e))].distanceTo(ps[cInd]);
					if(beVerbose)printf("OutArc %d: %d->Center %d with id %d:  cap %d cost: %lf\n", g.id(e),
											g.id(g.source(e)),
											target,cInd,
											capacity[e] ,cost[e]);
				}else{
					if(beVerbose)printf("OutArc %d: %d->%d:  cap %d cost: %lf\n", g.id(e),
									g.id(g.source(e)),
									g.id(g.target(e)),
									capacity[e] ,cost[e]);
				}
			}
			/*
			for(int j=0;j<k;j++){
				Node cnode = centerNodes[j];
				newArc =
				capacity[newArc] = 1;
				double costLocal= ps[i].distanceTo(ps[cPointsInds[j]]);
				cost[newArc] = costLocal;
			}*/
	} //end for all point nodes


	if(beVerbose)for (ArcIt a(g); a != INVALID; ++a) {
		//cout << "g.id(g.target(a)):" << g.id(g.target(a)) << endl;
		printf("End Arc %d: %d->%d:  cap %d cost: %lf\n", g.id(a),
						g.id(g.source(a)),
						g.id(g.target(a)),
						capacity[a] ,cost[a]);
		//cost[a]= 100;
		//capacity[a] = 5;
		//if it is not s or t
		//if(ns.flow(a)>0){
		/*
		if(g.id(g.source(a))<noDP){
				//WRONG!!indexT+1+i == centerIndex => centerID = node_CenterIndex-indexT-1
				int regionID = g.id(g.target(a))-indexT-1;
				regions[g.id(g.source(a))] = regionID;
					cout << "g.id(g.target(a)):" << g.id(g.target(a)) << endl;
					//if(beVerbose)
					printf("CS Region Map %d->%d\n",g.id(g.source(a))+1 ,regionID);
					cout << "g.id(g.source(a)):" << g.id(g.source(a)) << endl;
			}
		//}// end if flow
	*/
	}

	//cout << "capacityScale start until here:"<< getCurrentCPUDiff() << endl;

	//NetworkSimplex<SmartDigraph> ns(g);
	CostScaling<SmartDigraph,int,double> ns(g);

	//cout << "capacityScale end here:"<< getCurrentCPUDiff() << endl;

	//cout << "In Calc before run" << endl;
	//printDPs(ps, noDP);

	//cout << "capacityScale supply and run here:"<< getCurrentCPUDiff() << endl;
	ns.upperMap(capacity).costMap(cost).supplyMap(supply);
	//cout << "capacityScale run starts here:"<< getCurrentCPUDiff() << endl;
	// Run NetworkSimplex
	ns.run();
	// Print total flow cost
	//cout << "capacityScale run end here:"<< getCurrentCPUDiff() << endl;

	for (ArcIt a(g); a != INVALID; ++a) {
		//if it is not s or t
		if(ns.flow(a)>0){
			if(g.id(g.source(a))<noDP){
				//WRONG!!indexT+1+i == centerIndex => centerID = node_CenterIndex-indexT-1
				int regionID = g.id(g.target(a))-indexT-2;
				regions[g.id(g.source(a))] = g.id(g.target(a))-indexT-1;;

				if(beVerboseAboutFlow){
					cout << "g.id(g.target(a)):" << g.id(g.target(a)) << endl;
					//if(beVerbose)
					printf("CS Region Map %d->%d\n",g.id(g.source(a))+1 ,regionID);
					cout << "g.id(g.source(a)):" << g.id(g.source(a)) << endl;
				}
			}
		}
	}//end for arcIt a

	for(int i=0;i<noDP;i++){
		//printf("Region Map %d->%d\n");
		if(beVerbose)printf("index %d value %d\n",i+1,regions[i]);
	}

	//cout << "In Calc return " << endl;
	//printDPs(ps, noDP);
	if(beVerbose)printf("*****calcEnd********\n");
	return ns.totalCost<double>();

}

double calcRegionsNS2(const SmartDigraph& g,
		IntArcMap& capacity,
		DoubleArcMap& cost,
		IntNodeMap& supply,
		IntArcMap& name,
		const Node* nodesGraph,
		const Node* centerNodes,
		int indexT,
		int kToBeOutSwitched,
		int iToBeInSwitched,
		DataPoint ps[],int cPointsInds[], int noDP, int k, int cap, int* regions){

	//g.addNode();

	bool beVerboseAboutFlow= false;
	bool beVerbose = false;

	if(beVerbose)printf("Input graph created with %d nodes and %d arcs\n\n",
				countNodes(g), countArcs(g));
	Arc newArc;
	Node pnode;

	if(beVerbose){
	for(int z=0;z<k;z++)
		printf("cPointsInds[%d] = %d",z,cPointsInds[z]);
	}
	for(int i=0;i<noDP;i++){
			//// add arc s->p , cost 0, flowcap 1
			pnode = nodesGraph[ps[i].graphNodeIndex];
			//forall c add p->c cost dist, flowcap 1
			for (OutArcIt e(g, pnode); e!=INVALID; ++e){
				int target = g.id(g.target(e));
				if(target>noDP+1){
					target = target - noDP-2;
					int cInd = cPointsInds[target];
					cost[e] = ps[g.id(g.source(e))].distanceTo(ps[cInd]);
					if(beVerbose)printf("OutArc %d: %d->Center %d with id %d:  cap %d cost: %lf\n", g.id(e),
											g.id(g.source(e)),
											target,cInd,
											capacity[e] ,cost[e]);
				}else{
					if(beVerbose)printf("OutArc %d: %d->%d:  cap %d cost: %lf\n", g.id(e),
									g.id(g.source(e)),
									g.id(g.target(e)),
									capacity[e] ,cost[e]);
				}
			}
			/*
			for(int j=0;j<k;j++){
				Node cnode = centerNodes[j];
				newArc =
				capacity[newArc] = 1;
				double costLocal= ps[i].distanceTo(ps[cPointsInds[j]]);
				cost[newArc] = costLocal;
			}*/
	} //end for all point nodes


	if(beVerbose)for (ArcIt a(g); a != INVALID; ++a) {
		//cout << "g.id(g.target(a)):" << g.id(g.target(a)) << endl;
		printf("End Arc %d: %d->%d:  cap %d cost: %lf\n", g.id(a),
						g.id(g.source(a)),
						g.id(g.target(a)),
						capacity[a] ,cost[a]);
		//cost[a]= 100;
		//capacity[a] = 5;
		//if it is not s or t
		//if(ns.flow(a)>0){
		/*
		if(g.id(g.source(a))<noDP){
				//WRONG!!indexT+1+i == centerIndex => centerID = node_CenterIndex-indexT-1
				int regionID = g.id(g.target(a))-indexT-1;
				regions[g.id(g.source(a))] = regionID;
					cout << "g.id(g.target(a)):" << g.id(g.target(a)) << endl;
					//if(beVerbose)
					printf("CS Region Map %d->%d\n",g.id(g.source(a))+1 ,regionID);
					cout << "g.id(g.source(a)):" << g.id(g.source(a)) << endl;
			}
		//}// end if flow
	*/
	}
	bool verboseTime = false;
	if(verboseTime)cout << "capacityScale start until here:"<< getCurrentCPUDiff() << endl;

	NetworkSimplex<SmartDigraph> ns(g);
		//CapacityScaling<SmartDigraph> ns(g);

		//cout << "In Calc before run" << endl;
		//printDPs(ps, noDP);

	ns.upperMap(capacity).costMap(cost).supplyMap(supply);
		// Run NetworkSimplex
	ns.run();


	if(verboseTime)cout << "NetworkSimplex supply and run here:"<< getCurrentCPUDiff() << endl;
	ns.upperMap(capacity).costMap(cost).supplyMap(supply);
	if(verboseTime)cout << "NetworkSimplex run starts here:"<< getCurrentCPUDiff() << endl;
	// Run NetworkSimplex
	ns.run();
	// Print total flow cost
	if(verboseTime)cout << "capacityScale run end here:"<< getCurrentCPUDiff() << endl;

	for (ArcIt a(g); a != INVALID; ++a) {
		//if it is not s or t
		if(ns.flow(a)>0){
			if(g.id(g.source(a))<noDP){
				//WRONG!!indexT+1+i == centerIndex => centerID = node_CenterIndex-indexT-1
				int regionID = g.id(g.target(a))-indexT-2;
				regions[g.id(g.source(a))] = g.id(g.target(a))-indexT-1;;

				if(beVerboseAboutFlow){
					cout << "g.id(g.target(a)):" << g.id(g.target(a)) << endl;
					//if(beVerbose)
					printf("CS Region Map %d->%d\n",g.id(g.source(a))+1 ,regionID);
					cout << "g.id(g.source(a)):" << g.id(g.source(a)) << endl;
				}
			}
		}
	}//end for arcIt a

	for(int i=0;i<noDP;i++){
		//printf("Region Map %d->%d\n");
		if(beVerbose)printf("index %d value %d\n",i+1,regions[i]);
	}

	//cout << "In Calc return " << endl;
	//printDPs(ps, noDP);
	if(beVerbose)printf("*****calcEnd********\n");
	return ns.totalCost<double>();

}



double calcRegionsCSRG(const SmartDigraph& g,
		IntArcMap& capacity,
		DoubleArcMap& cost,
		IntNodeMap& supply,
		IntArcMap& name,
				const Node* nodesGraph,
				const Node* centerNodes,
				int indexT,
				int kToBeOutSwitched,
				int iToBeInSwitched,
				DataPoint ps[],int cPointsInds[], int noDP, int k, int cap, int* regions){


	return calcRegionsCS( ps, cPointsInds,  noDP,  k, cap, regions);

}


//returns cost of mapping to given centers, regions is map #index of DP to index in DP of center
double calcRegionsCS(DataPoint ps[],int cPointsInds[], int noDP, int k, int cap, int* regions){

	bool beVerbose = false;
	bool lilVerbose = false;
	bool beVerboseAboutFlow = false;
	bool graphVerbose = false;
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
	Node nodesGraph[noDP+2+k];
	Node centerNodes[k];
	//for all points
	//add vertex
	for(int i=0;i<noDP;i++){
		ps[i].graphNodeIndex = i;
		nodesGraph[i] = g.addNode();
		supply[nodesGraph[i]] = 0;
	}

	// add s and t
	nodesGraph[noDP] = g.addNode();
	nodesGraph[noDP+1] = g.addNode();
	int indexS = noDP;
	int indexT = noDP+1;

	//make note of centers for later
	int cPoints[k];
	int counter = 0;
	/*
	for(int i=0;i<noDP;i++){
		if (ps[i].isCenter){
			cPoints[counter]=i;
			centerNodes[counter]=g.addNode();
			nodesGraph[indexT+1+i]=centerNodes[counter];
			counter++;
		}
	}*/
	for(int i=0;i<k;i++) {
		cPoints[i] = cPointsInds[i];
		centerNodes[i]=g.addNode();
		nodesGraph[indexT+1+i]=centerNodes[i];
	}

	int demand = (noDP);
	if(beVerbose)cout << "demand:" << demand << endl;
	supply[nodesGraph[indexS]] = (demand);
	supply[nodesGraph[indexT]] = -(demand);

	int numberOfArcs = (noDP)*(k+1)+k;
	if(beVerbose)cout << "expected numberOfArcs:" << numberOfArcs << endl;

	int arcCounter = 0;
	Arc newArc;
	Node pnode;
	//for all points
	for(int i=0;i<noDP;i++){
			//// add arc s->p , cost 0, flowcap 1
			pnode = nodesGraph[ps[i].graphNodeIndex];
			newArc = g.addArc(nodesGraph[indexS],pnode);
			capacity[newArc] = 1;
			cost[newArc] = 0;
			arcCounter++;
			//arcNames[arcCounter] = "s->p";//string("S -> node") + i;
			//forall c add p->c cost dist, flowcap 1
			for(int j=0;j<k;j++){
				Node cnode = centerNodes[j];
				newArc = g.addArc(pnode, cnode);
				//if(beVerbose)cout << g.id(g.source(newArc)) << "->" << g.id(g.target(newArc))<< endl;
				capacity[newArc] = 1;
				double costLocal= ps[i].distanceTo(ps[cPoints[j]]);
				//if(beVerbose)ps[i].print();
				//if(beVerbose)ps[cPoints[j]].print();
				//if(beVerbose)cout<< "ArcCost:"<< costLocal<< endl;
				cost[newArc] = costLocal;
				arcCounter++;
				//string tempS = "p->c";string("p: node") + i +"-> c: node "+ j +" cost:" + ps[i].distanceTo(cPoints[j]);
				//arcNames[arcCounter] ="p->c";;
			}
	} //end for all point nodes
	for(int j=0;j<k;j++){
		//// add arc c->t , cost 0, flowcap cap
		pnode = centerNodes[j];
		newArc = g.addArc(pnode,nodesGraph[indexT]);
		capacity[newArc] = cap;
		cost[newArc] = 0;
		arcCounter++;
		//arcNames[arcCounter] = "c->t";//string("c: node") + std::to_string(i) + "-> t";
	}//for all center nodes

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

	if(beVerbose)std::cout << "Total cost: " << ns.totalCost<double>() << std::endl;

	// Print flow values on the arcs with ArcIterator
	if(beVerbose)printf("Flow values on arcs:\n");
	for (ArcIt a(g); a != INVALID; ++a) {
		//if it is not s or t
		if(ns.flow(a)>0){
			if(g.id(g.source(a))<noDP){
				//WRONG!!indexT+1+i == centerIndex => centerID = node_CenterIndex-indexT-1

				int regionID = g.id(g.target(a))-indexT-1;
				regions[g.id(g.source(a))] = regionID;

				if(beVerboseAboutFlow){
					cout << "g.id(g.target(a)):" << g.id(g.target(a)) << endl;
					//if(beVerbose)
					printf("CS Region Map %d->%d\n",g.id(g.source(a))+1 ,regionID);
					cout << "g.id(g.source(a)):" << g.id(g.source(a)) << endl;
				}

				/*
				if(g.id(g.target(a))<noDP){
					if(beVerbose)printf("Region Map %d->%d\n",g.id(g.source(a))+1 ,g.id(g.target(a))+1);
					regions[g.id(g.source(a))] = g.id(g.target(a));
				}else{
					if(beVerbose)printf("Region Map %d-is Center\n",g.id(g.source(a))+1);
					regions[g.id(g.source(a))] = g.id(g.source(a));
				}*/
			}
		}

		if(beVerbose||graphVerbose)printf("Arc %d: %s->%s:  %d/%d cost: %lf\n", g.id(a),
				vertNoToString(g.id(g.source(a)),noDP).c_str(),
				vertNoToString(g.id(g.target(a)),noDP).c_str(),
				ns.flow(a), capacity[a] ,cost[a]);

	}
	for(int i=0;i<noDP;i++){
		//printf("Region Map %d->%d\n");
		if(beVerbose)printf("index %d value %d\n",i+1,regions[i]);
	}

	//cout << "In Calc return " << endl;
	//printDPs(ps, noDP);
	if(beVerbose||lilVerbose)printf("*****calcEnd********\n");
	return ns.totalCost<double>();
}


double bruteForceSearch(DataPoint ps[], int noDP, int k, int cap,int regionsBest[],int cPointsBest[]){
	//start with random centers
	int cPoints[k];
	//int cPointsNew[k];
	int regions[noDP];
	//int regionsNew[noDP];

	bool beVerbose = false;

    int indArray[noDP];
    for(int j=0;j<noDP;j++) indArray[j]=j;

    std::list<int> lt;
    std::list<std::list<int>> all;
    subset(indArray,noDP,k,0,lt,all);
    if(beVerbose)cout << "ALL--------------" << endl;
    if(beVerbose)printList(all);

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
        	//cout << " " << *it;
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

			if(cPointsBest!=NULL && regionsBest!=NULL){
				for(int p=0;p<k;p++){
					cPointsBest[p]=cPoints[p];
				}
				for(int v=0;v<noDP;v++){
					regionsBest[v]=regions[v];
				}

			}
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
	int noCMDs = 10;
	const char * commandsForGnuplot[] = {
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

	printf("set xrange [%d:%d]\n", plotmin, plotmax);
	fprintf(gnuplotPipe, "set xrange [%d:%d]\n", plotmin-1, plotmax+1);
	fprintf(gnuplotPipe, "set yrange [%d:%d]\n", plotmin-1, plotmax+1);

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
		fprintf(gnuplotPipe, "%lf %lf\n", dps[centers[i]].X, dps[centers[i]].Y);
	}
	fprintf(gnuplotPipe, "e\n");
	int j;
	for (j=0; j < noDP; j++)
	{
		//fprintf(gnuplotPipe, "plot '-' w p ls %d t \"DataPoints %d \" \n",dps[regions[j]].centerID%9+1,dps[regions[j]].centerID%9+1);
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
		if(vertexNo<maxNo+2){
			if(vertexNo==maxNo) retString = "S";
					else retString = "T";
		}else
			retString = my_to_string(vertexNo-maxNo-1);
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


void updateDPS(DataPoint ps[],int noDP,int center[],int k, int regions[]){
	 for(int j=0;j<noDP;j++){
		ps[j].isCenter = false;
		ps[j].centerID = -1;
		ps[j].indexOfmappedCenter = regions[j];

	 }
	 for(int j=0;j<k;j++){
		int ind = center[j];
		cout << "center Index:" <<ind << endl;
		ps[ind].isCenter = true;
		ps[ind].centerID = j;
	 }
}

double calculateCost(DataPoint ps[],int noDP,int center[]){
	double sum = 0;
	//cout << "calcCost" << endl;
	for(int j=0;j<noDP;j++){
		DataPoint c = ps[center[ps[j].indexOfmappedCenter]];
		/*
		cout << "c is:" << endl;
		c.print();
		cout << "p is:" << endl;
		ps[j].print();
		*/
		sum = sum + ps[j].distanceTo(c);
	 }
	cout << "calcCost--------------end" << endl;
	return sum;
}

//end of helper functions *******************************
