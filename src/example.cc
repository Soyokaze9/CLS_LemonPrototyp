/*
#include <iostream>
#include <cstdio>
#include <fstream>
#include <lemon/smart_graph.h>
#include <lemon/lgf_reader.h>
#include <lemon/lgf_writer.h>
#include <lemon/concepts/graph.h>
#include <lemon/smart_graph.h>
#include <lemon/network_simplex.h>
#include <lemon/dimacs.h>
#include <lemon/list_graph.h>

#include <unistd.h>
#define GetCurrentDir getcwd


using namespace lemon;
using namespace std;


int example0();
int example1();
int example2();
int example4();


std::string GetCurrentWorkingDir( void ) {
  char buff[FILENAME_MAX];
  GetCurrentDir( buff, FILENAME_MAX );
  std::string current_working_dir(buff);
  return current_working_dir;
}

int start() {
	example0();
	return 0;
}


int example0() {
  SmartDigraph g;
  SmartDigraph::ArcMap<int> cap(g);
  SmartDigraph::Node s, t;

  std::cout << GetCurrentWorkingDir() << std::endl;

  try {
    digraphReader(g, "digraph.lgf"). // read the directed graph into g
      arcMap("capacity", cap).       // read the 'capacity' arc map into cap
      node("source", s).             // read 'source' node to s
      node("target", t).             // read 'target' node to t
      run();
  } catch (Exception& error) { // check if there was any error
    std::cerr << "Error: " << error.what() << std::endl;
    return -1;
  }

  std::cout << "A digraph is read from 'digraph.lgf'." << std::endl;
  std::cout << "Number of nodes: " << countNodes(g) << std::endl;
  std::cout << "Number of arcs: " << countArcs(g) << std::endl;

  std::cout << "We can write it to the standard output:" << std::endl;

  digraphWriter(g).                // write g to the standard output
    arcMap("capacity", cap).       // write cap into 'capacity'
    node("source", s).             // write s to 'source'
    node("target", t).             // write t to 'target'
    run();

  return 0;
}



//classes
class DataPoint{

public:
	int X;
	int Y;
	bool isCenter;
	int graphNodeIndex;
	DataPoint* nearestCenter;
	DataPoint(){
		X = 0;
		Y = 0;
		isCenter = false;
		nearestCenter = NULL;
	}
	DataPoint(int x, int y){
		X = x;
		Y = y;
		isCenter = false;
		nearestCenter = NULL;
	}
	int distanceTo(DataPoint p){
        int dx = abs(X - p.X);
        int dy = abs(Y - p.Y);
        //return max(dx,dy);
        return sqrt(dx*dx + dy*dy);
	}
};

int example1()
{
  //lemon::ListGraph g;
  ListDigraph g;


  ListDigraph::Node u = g.addNode();
  ListDigraph::Node v = g.addNode();
  ListDigraph::Arc  a = g.addArc(u, v);
  cout << "Hello World! This is LEMON library here." << endl;
  cout << "We have a directed graph with " << countNodes(g) << " nodes "
       << "and " << countArcs(g) << " arc." << endl;
  return 0;

}

int example2() {

  // Create a directed graph
  DIGRAPH_TYPEDEFS(SmartDigraph);
  SmartDigraph g;

  // Build a simple graph
  Node A = g.addNode();
  Node B = g.addNode();
  Node C = g.addNode();
  Node D = g.addNode();
  IntNodeMap supply(g);
  supply[A] = 20;
  supply[B] = 0;
  supply[C] = 0;
  supply[D] = -20;

  Arc a1 = g.addArc(A, B);
  Arc a2 = g.addArc(A, C);
  Arc a3 = g.addArc(B, C);
  Arc a4 = g.addArc(B, D);
  Arc a5 = g.addArc(C, D);
  IntArcMap capacity(g), cost(g);
  capacity[a1] = 10;
  cost[a1] = 100;
  capacity[a2] = 20;
  cost[a2] = 300;
  capacity[a3] = 5;
  cost[a3] = 50;
  capacity[a4] = 10;
  cost[a4] = 200;
  capacity[a5] = 20;
  cost[a5] = 100;

  printf("Input graph created with %d nodes and %d arcs\n\n",
      countNodes(g), countArcs(g));

  // Initialize NetworkSimplex algorithm object
  NetworkSimplex<SmartDigraph> ns(g);
  ns.upperMap(capacity).costMap(cost).supplyMap(supply);

  // Run NetworkSimplex
  ns.run();

  // Print total flow cost
  printf("Total flow cost: %d\n\n", ns.totalCost());

  // Print flow values on the arcs
  printf("Flow values on arcs:\n");
  for (ArcIt a(g); a != INVALID; ++a) {
    printf("Arc %d: %d/%d\n", g.id(a), ns.flow(a), capacity[a]);
  }

  return 0;
}


int example3() {

  // Create a directed graph
  DIGRAPH_TYPEDEFS(SmartDigraph);
  SmartDigraph g;

  // Create data structures (i.e. maps) associating values to nodes and arcs of the graph
  IntArcMap lower(g), capacity(g), cost(g);
  IntNodeMap supply(g);

  // Read DIMACS input file
  ifstream input("example.dim");
  readDimacsMin(input, g, lower, capacity, cost, supply);
  input.close();

  // Initialize NetworkSimplex algorithm object
  NetworkSimplex<SmartDigraph> ns(g);
  ns.lowerMap(lower).upperMap(capacity).costMap(cost).supplyMap(supply);

  // Run NetworkSimplex
  ns.run();

  // Print total flow cost
  printf("Total flow cost: %d\n\n", ns.totalCost());

  // Print flow values on the arcs
  printf("Flow values on arcs:\n");
  for (ArcIt a(g); a != INVALID; ++a) {
    printf("Arc %d: %d/%d\n", g.id(a), ns.flow(a), capacity[a]);
  }

  return 0;
}

int example4(){
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


    //create graph
    DIGRAPH_TYPEDEFS(SmartDigraph);
    SmartDigraph g;
    //map for cost and capacity
    IntArcMap capacity(g), cost(g);
    IntNodeMap supply(g);
    //Digraph::NodeMap<string>
    IntArcMap name(g);
    //StringArcMap name(g);

    Node nodesGraph[noDP+2];
    counter = 0;
    //for all points
    	//add vertex
    for(int i=0;i<noDP;i++){
    	cout << "i DP: " << i << endl;
    	ps[i].graphNodeIndex = i;
    	nodesGraph[i] = g.addNode();
    	supply[nodesGraph[i]] = 0;
    }

    cout << "noDP: " << noDP << endl;
    for(int i=0;i<noDP;i++){
    	if (ps[i].isCenter){
    		cPoints[counter]=ps[i];
			cout << "centerCounter: " << counter << endl;
			cout << "centerID:" <<i<<endl;
			counter++;
    	}
    }

    for(int j=0;j<k;j++){
    	cout << "cPoints[j].graphNodeIndex:  " << cPoints[j].graphNodeIndex << endl;
    }

    for(int i=0;i<noDP;i++){
    	cout << "i DP: " << i << endl;
    	cout << "ps[i].graphNodeIndex:  " << ps[i].graphNodeIndex << endl;
    }

    //s,t
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
    //string arcNames[numberOfArcs];

    int arcCounter = 0;

    //for all points
    for(int i=0;i<noDP;i++){
    	Arc newArc;
    	Node pnode;
    	stringstream ss;
    	cout << "DP no:  " << i << endl;
    	cout << "ps[i].graphNodeIndex:  " << ps[i].graphNodeIndex << endl;

    	//int nodeIdx = ps[i].graphNodeIndex;
    	if (ps[i].isCenter){
    		//// add arc c->t , cost 0, flowcap cap
    		pnode = nodesGraph[ps[i].graphNodeIndex];
    		//cout << "pnode:" << pnode << endl;
    		newArc = g.addArc(pnode,nodesGraph[indexT]);
    		cout << "ps isCenter" << endl;
    		cout << g.id(g.source(newArc)) << "->" << g.id(g.target(newArc))<< endl;
    		capacity[newArc] = cap;
    		cost[newArc] = 0;
    		//name[newArc] = arcCounter;
    		arcCounter++;
    		//arcNames[arcCounter] = "c->t";//string("c: node") + std::to_string(i) + "-> t";
    	}else{
    		//// add arc s->p , cost 0, flowcap 1
    		pnode = nodesGraph[ps[i].graphNodeIndex];
    		//cout << "pnode:" << pnode << endl;
    		newArc = g.addArc(nodesGraph[indexS],pnode);
    		cout << "ps isNOTCenter" << endl;
    		cout << g.id(g.source(newArc)) << "->" << g.id(g.target(newArc))<< endl;
    		capacity[newArc] = 1;
    		cost[newArc] = 0;
    		//name[newArc] = arcCounter;
    		arcCounter++;
    		//arcNames[arcCounter] = "s->p";//string("S -> node") + i;
    		//forall c add p->c cost dist, flowcap 1
    		for(int j=0;j<k;j++){
    			cout << "point:"<<i << "center:"<<j<<endl;
    			cout << "ps[i].graphNodeIndex:  " << ps[i].graphNodeIndex << endl;
    			cout << "cPoints[j].graphNodeIndex:  " << ps[i].graphNodeIndex << endl;

    			Node cnode = nodesGraph[cPoints[j].graphNodeIndex];
    			//cout << "pnode:" << pnode << endl;
    			//cout << "cnode:" << cnode << endl;
        		newArc = g.addArc(pnode, cnode);
        		cout << g.id(g.source(newArc)) << "->" << g.id(g.target(newArc))<< endl;
        		capacity[newArc] = 1;
        		cost[newArc] = ps[i].distanceTo(cPoints[j]);
        		//name[newArc] = arcCounter;
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

    // Print flow values on the arcs
    printf("Flow values on arcs:\n");
    for (ArcIt a(g); a != INVALID; ++a) {
      printf("Arc %d: %d->%d:  %d/%d cost: %d\n", g.id(a),g.id(g.source(a)),g.id(g.target(a)), ns.flow(a), capacity[a] ,cost[a]);
    }

    return 0;
}
*/
