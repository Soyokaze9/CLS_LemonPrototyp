#include <iostream>
#include<lemon/list_graph.h>

#include <cstdio>
#include <fstream>

#include <lemon/concepts/graph.h>
#include <lemon/smart_graph.h>
#include <lemon/network_simplex.h>
#include <lemon/dimacs.h>

//classes
class DataPoint{

public:
	double X;
	double Y;
	bool isCenter;
	int graphNodeIndex;
	DataPoint* nearestCenter;
	DataPoint(){
		X = 0;
		Y = 0;
		isCenter = false;
		nearestCenter = NULL;
		graphNodeIndex = -1;
	}
	DataPoint(double x, double y){
		X = x;
		Y = y;
		isCenter = false;
		nearestCenter = NULL;
		graphNodeIndex = -1;
	}
	double distanceTo(DataPoint p){
        double dx = abs(X - p.X);
        double dy = abs(Y - p.Y);
        //return max(dx,dy);
        return sqrt(dx*dx + dy*dy);
	}
};

int example1();
int example2();
int example3();
void exampleSearch();
void exampleLoopWithPlot();
double localSearch(DataPoint ps[], int noDP, int k, int cap);
void directPlot(DataPoint* dps, int noDP);
void directPlotRegions(DataPoint* dps, int noDP,DataPoint* centers, int noCenters,int regions[],char * title);
void directPlotPoints(DataPoint* dps, int noDP,DataPoint* centers, int noCenters,char * title);
double calcRegions(DataPoint* dps, int noDP, int k, int cap, int regions[]);
std::string vertNoToString(int vertexNo,int maxNo);

using namespace lemon;
using namespace std;

int main() {
	int no = 5;
    double xvals[no] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double yvals[no] = {5.0 ,3.0, 1.0, 3.0, 5.0};
    DataPoint dps[no];
    for(int i=0;i<no;i++){
    	dps[i] = DataPoint(xvals[i],yvals[i]);
    }
    //exampleLoopWithPlot();
    exampleSearch();
}

double localSearch(DataPoint ps[], int noDP, int k, int cap){
	//start with random centers
	DataPoint cPoints[k];
	DataPoint cPointsNew[k];
	for(int i=0;i<k;i++){
    	int randInt = rand() % noDP;
    	ps[randInt].isCenter = true;
    	cPoints[i] = ps[randInt];
    	cPointsNew[i] = ps[randInt];
    	cout << "cPoints[i]: x " << cPoints[i].X << " y:"<< cPoints[i].Y << endl;
    }

    int regions[noDP];
    double currentCost = calcRegions(ps, noDP, k,cap,regions);
    double newCost;
    double improvement;
    directPlotPoints(ps, noDP,cPoints, k, "Start");
    directPlotRegions(ps, noDP,cPoints, k,regions,"Start");

    //notAllTested = true;
    //while(notAllTested){}

    bool forBreak = false;
	for(int j=0;j<k;j++){
		for(int l=0;l<noDP;l++){
			//try swap
			cPointsNew[j] = ps[l];
			cPoints[j].isCenter = false;
			cPointsNew[j].isCenter = true;

			newCost = calcRegions(ps, noDP, k,cap,regions);
		    //directPlotRegions(ps, noDP,cPoints, k,regions);
			if(newCost<currentCost){
				cPoints[j] = cPointsNew[j];
				currentCost=newCost;
				forBreak = true;
				break;
				//goto stop;
			}else{
				cPoints[j].isCenter = true;
				cPointsNew[j] = cPoints[j];
			}
		}
		if(forBreak)break;
	}

    // This message does not print:
    printf( "Loop exited.");

    //stop:
    printf("Jumped to stop" );

	return 0;

}

void directPlot(DataPoint* dps, int noDP)
{
    //char * commandsForGnuplot[] = {"set title \"TITLE\"", "plot 'data.temp'"};
    int noCMDs = 3;
	char * commandsForGnuplot[] = {"set title \"TITLE\"", "set xrange [0:6]","set yrange [0:6]"};

    FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");

    for (int j=0; j < noCMDs;j++)
    {
    	fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[j]); //Send commands to gnuplot
    }

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

void directPlotPoints(DataPoint* dps, int noDP,DataPoint* centers, int noCenters, char * title)
{

	int noCMDs = 9;


	char * commandsForGnuplot[] = {
			"set xrange [0:10]",
			"set yrange [0:10]",
	"set style line 1 lc rgb 'black' pt 5 ps 1  # square",
	"set style line 2 lc rgb 'red' pt 7 ps 2  # circle",
	"set style line 3 lc rgb 'blue' pt 9   # triangle",
	"set style line 4 lc rgb 'black' pt 1   # plus sign",
	"set style line 5 lc rgb 'red' pt 2   # cross",
	"set style line 6 lc rgb 'black' pt 10   # triangle down",
	"set style line 7 lc rgb 'blue' pt 16   #star"};

	FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");

	fprintf(gnuplotPipe, "set title %s \n", title);

    for (int j=0; j < noCMDs;j++)
    {
    	fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[j]); //Send commands to gnuplot
    }

    fprintf(gnuplotPipe, "plot '-' w p ls 2 t  \"Center\" , '-' w p ls 1 t \"DataPoints\" \n");
    int i;

    for (i=0; i < noCenters; i++)
    {
    	fprintf(gnuplotPipe, "%lf %lf\n", centers[i].X, centers[i].Y);
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

void directPlotRegions(DataPoint* dps, int noDP,DataPoint* centers, int noCenters,int regions[],char * title)
{

	int ColorCode[noDP];

	int noCMDs = 11;
	char * commandsForGnuplot[] = {
			"set xrange [0:10]",
			"set yrange [0:10]",
	"set style line 1 lc rgb 'black' pt 5 ps 1  # square",
	"set style line 2 lc rgb 'red' pt 7 ps 2  # circle",
	"set style line 3 lc rgb 'blue' pt 9   # triangle",
	"set style line 4 lc rgb 'black' pt 1   # plus sign",
	"set style line 5 lc rgb 'red' pt 2   # cross",
	"set style line 6 lc rgb 'black' pt 10   # triangle down",
	"set style line 7 lc rgb 'blue' pt 16   #star",
	"set style line 8 lc rgb 'red' pt 16   #star",
	"set style line 9 lc rgb 'red' pt 9   # triangle",
	};

	FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");

	fprintf(gnuplotPipe, "set title %s \n", title);
    for (int j=0; j < noCMDs;j++)
    {
    	fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[j]); //Send commands to gnuplot
    }
    //set multiplot
    fprintf(gnuplotPipe, "set multiplot\n");
    fprintf(gnuplotPipe, "plot '-' w p ls 2 t  \"Center\" \n" ); //, '-' w p ls 1 t \"DataPoints\" \n");
    int i;
    for (i=0; i < noCenters; i++)
    {
    	//printf("plotting center point: %lf %lf\n", centers[i].X, centers[i].Y);
    	fprintf(gnuplotPipe, "%lf %lf\n", centers[i].X, centers[i].Y);
    }
    fprintf(gnuplotPipe, "e\n");
    int j;
    for (j=0; j < noDP; j++)
    {
    	fprintf(gnuplotPipe, "plot '-' w p ls %d t \"DataPoints %d \" \n",regions[j]%8,regions[j]%8);
    	fprintf(gnuplotPipe, "%lf %lf\n", dps[j].X, dps[j].Y);
    	fprintf(gnuplotPipe, "e\n");
    }
    fflush(gnuplotPipe);
    fprintf(gnuplotPipe, "unset multiplot\n");
    pclose(gnuplotPipe);
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
    p6.isCenter = true;
    p7.isCenter = true;
    DataPoint ps[noDP] = {p1,p2,p3,p4,p5,p6,p7,p8};
    DataPoint cPoints[k];
    int counter = 0;
    for(int i=0;i<noDP;i++){
    	if (ps[i].isCenter){
    		cPoints[counter]=ps[i];
			//cout << "Center:" << endl;
			//cout << "X:" << cPoints[counter].X << "Y:" << cPoints[counter].Y <<endl;
			//cout << "X:" << ps[i].X << "Y:" << ps[i].Y <<endl;
			counter++;
    	}
    }
    int regions[noDP];
    double newCost = calcRegions(ps, noDP, k,cap,regions);
    directPlotPoints(ps, noDP,cPoints, k, "Example");
    directPlotRegions(ps, noDP,cPoints, k,regions,"Expample");
}

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
    return calcRegions(ps, noDP, k,cap,regions);
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
    int regions[noDP];
    return calcRegions(ps, noDP, k,cap,regions);
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
    calcRegions(ps, noDP, k,cap,regions);
    printf("new region\n");
    for(int i=0;i<noDP;i++){
    	//printf("Region Map %d->%d\n");
    	printf("index %d value %d\n",i+1,(regions[i]+1)>noDP?0:regions[i]+1);
    }

    return 0;
}

double calcRegions(DataPoint ps[], int noDP, int k, int cap, int regions[]){
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
    DataPoint cPoints[k];
    int counter = 0;
    for(int i=0;i<noDP;i++){
    	if (ps[i].isCenter){
    		cPoints[counter]=ps[i];
			counter++;
    	}
    }
    // add s and t
    nodesGraph[noDP] = g.addNode();
    nodesGraph[noDP+1] = g.addNode();
    int indexS = noDP;
    int indexT = noDP+1;

    int demand = (noDP-k);
    cout << "demand:" << demand << endl;
    supply[nodesGraph[indexS]] = (demand);
    supply[nodesGraph[indexT]] = -(demand);

    int numberOfArcs = (noDP-k)*(k+1)+k;
    cout << "numberOfArcs:" << numberOfArcs << endl;

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
    			Node cnode = nodesGraph[cPoints[j].graphNodeIndex];
        		newArc = g.addArc(pnode, cnode);
        		cout << g.id(g.source(newArc)) << "->" << g.id(g.target(newArc))<< endl;
        		capacity[newArc] = 1;
        		cost[newArc] = ps[i].distanceTo(cPoints[j]);
        		arcCounter++;
        		//string tempS = "p->c";string("p: node") + i +"-> c: node "+ j +" cost:" + ps[i].distanceTo(cPoints[j]);
        		//arcNames[arcCounter] ="p->c";;
    		}
    	}
    }
    printf("Input graph created with %d nodes and %d arcs\n\n",
        countNodes(g), countArcs(g));
    // Initialize NetworkSimplex algorithm object
    NetworkSimplex<SmartDigraph> ns(g);


    ns.upperMap(capacity).costMap(cost).supplyMap(supply);
    // Run NetworkSimplex
    ns.run();
    // Print total flow cost
    printf("Total flow cost: %d\n\n", ns.totalCost());

    // Print flow values on the arcs with ArcIterator
    printf("Flow values on arcs:\n");
    for (ArcIt a(g); a != INVALID; ++a) {
    	//if it is not s or t
    	if(ns.flow(a)>0){
        	if(g.id(g.source(a))<noDP){
        		if(g.id(g.target(a))<noDP){
            		printf("Region Map %d->%d\n",g.id(g.source(a))+1 ,g.id(g.target(a))+1);
            		regions[g.id(g.source(a))] = g.id(g.target(a));
        		}else{
            		printf("Region Map %d-is Center\n",g.id(g.source(a))+1);
            		regions[g.id(g.source(a))] = g.id(g.source(a));
        		}
        	}
    	}
    	printf("Arc %d: %s->%s:  %d/%d cost: %lf\n", g.id(a),
    		  vertNoToString(g.id(g.source(a)),noDP).c_str(),
			  vertNoToString(g.id(g.target(a)),noDP).c_str(),
			  ns.flow(a), capacity[a] ,cost[a]);
    }
    for(int i=0;i<noDP;i++){
    	//printf("Region Map %d->%d\n");
    	printf("index %d value %d\n",i+1,regions[i]+1);
    }
    return ns.totalCost();
}

std::string to_string(int Number){
	string res;
	ostringstream oss;
	oss << Number;
	res = oss.str();
	return res;
}

std::string vertNoToString(int vertexNo,int maxNo){

	std::string retString ="";
	if(vertexNo<maxNo){
		retString = to_string(vertexNo+1);
	}else{
		if(vertexNo==maxNo) retString = "S";
		else retString = "T";
	}
	return retString;
}
