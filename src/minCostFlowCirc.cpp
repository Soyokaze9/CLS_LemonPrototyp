#include <lemon/smart_graph.h>
#include <lemon/lgf_reader.h>
#include <lemon/lgf_writer.h>
#include <lemon/list_graph.h>
#include <lemon/cycle_canceling.h>

#include <lemon/preflow.h>


namespace lemon
{
	double Tolerance<double>::def_epsilon = 1e-5;
}

using namespace lemon;
typedef ListDigraph Graph;
typedef int LimitValueType;

void flowCycleCanceling()
{
	/**
	 * calculate min cost max flow via cycle canceling
	 * sample graph (arcs are dircted from left to right
	 *
	 *     2     5
	 *    / \   / \
	 *   /   \ /   \
	 *  0     4--6--1
	 *   \   / \   /
	 *    \ /   \ /
	 *     3     7
	 *
	 * vertex 0 represents the source, 1 target
	 * cost are set to 1, ecept for source and target arcs
	 * cost for (3,4) = 2
	 */

	Graph g;

	// lower and upper bounds, cost
	ListDigraph::ArcMap<LimitValueType> l_i(g), u_i(g), c_i(g);

	// source and sink vertices
	Graph::Node n0 = g.addNode(), n1 = g.addNode();

	// other nodes/vertices
	Graph::Node n2 = g.addNode(), n3 = g.addNode(), n4 = g.addNode(), n5 = g.addNode(),
		n6 = g.addNode(), n7 = g.addNode();

	Graph::Arc a02 = g.addArc(n0, n2), a03 = g.addArc(n0, n3), a24 = g.addArc(n2, n4),
			a34 = g.addArc(n3, n4), a45 = g.addArc(n4, n5), a46 = g.addArc(n4, n6),
			a47 = g.addArc(n4, n7), a51 = g.addArc(n5, n1), a61 = g.addArc(n6, n1),
			a71 = g.addArc(n7, n1);

	static const Graph::Arc arcsArray[] = { a02, a03, a24, a34, a45, a46, a47, a51, a61, a71 };

	//std::vector<Graph::Arc> arcs (arcsArray, sizeof(arcsArray)/sizeof(arcsArray[0]));

        // arcs for circulation! (source to target and vice versa)
	Graph::Arc ts = g.addArc(n1, n0);
	Graph::Arc st = g.addArc(n0, n1);


	// graph must be finished before initializing cycle canceling !
	CycleCanceling<Graph, LimitValueType, LimitValueType> cycle(g);

        // bounds for circulation arcs
	l_i[ts] = 0; u_i[ts] = cycle.INF; c_i[ts] = 0;
	l_i[st] = 0; u_i[ts] = cycle.INF; c_i[ts] = 0;

        // set cost for arcs
	/*
	for ( auto Arc & a : arcs )
	{
		l_i[a] = 1;
		u_i[a] = cycle.INF;
		c_i[a] = (g.source(a) == n0|| g.target(a) == n1 ) ? 0 : 1;
	}
	*/

	for(int h=0;h<10;h++){
		Graph::Arc a = arcsArray[h];
		l_i[a] = 1;
		u_i[a] = cycle.INF;
		c_i[a] = (g.source(a) == n0|| g.target(a) == n1 ) ? 0 : 1;

	}

	c_i[a34] = 2; // arc (3,4)

        //set lower/upper bounds, cost
	cycle.lowerMap(l_i).upperMap(u_i).costMap(c_i);

	CycleCanceling<Graph>::ProblemType ret = cycle.run();

	// get flow of arcs
	ListDigraph::ArcMap<LimitValueType> flow(g);
	cycle.flowMap(flow);

	digraphWriter(g).                  // write g to the standard output
	        arcMap("cost", c_i).          // write 'cost' for for arcs
	        arcMap("flow", flow).          // write 'flow' for for arcs
	        arcMap("l_i", l_i).
	        arcMap("u_i", u_i).
	        node("source", n0).             // write s to 'source'
	        node("target", n1).             // write t to 'target'
	        run();

	switch ( ret )
	{
	case CycleCanceling<Graph>::INFEASIBLE:
		std::cerr << "INFEASIBLE" << std::endl;
	break;
	case CycleCanceling<Graph>::OPTIMAL:
		std::cerr << "OPTIMAL" << std::endl;
	break;
	case CycleCanceling<Graph>::UNBOUNDED:
		std::cerr << "UNBOUNDED" << std::endl;
	break;
	}

	// determine flow from source and flow to target
	double flowToT = 0, flowToS = 0;
	for (ListDigraph::ArcMap<LimitValueType>::ItemIt a(flow); a != INVALID; ++a)
	{
		if ( g.target(a) == n1 )
			flowToT += cycle.flow(a);

		if ( g.source(a) == n0 )
			flowToS += cycle.flow(a);
	}

	std::cerr << "flow to t = " << flowToT << " flow from s = " << flowToS << std::endl;
	std::cerr << "total cost = " << cycle.totalCost<double>() << std::endl;

}
