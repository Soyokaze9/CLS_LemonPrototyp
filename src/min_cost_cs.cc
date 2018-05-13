/*
 *
 * */
#include <iostream>
#include <lemon/smart_graph.h>
#include <lemon/lgf_reader.h>
#include <lemon/lgf_writer.h>
#include <lemon/capacity_scaling.h>

using namespace lemon;

int mainCS() {
    SmartDigraph g;
    SmartDigraph::ArcMap<int> cap(g);
    SmartDigraph::ArcMap<int> cost(g);
    SmartDigraph::ArcMap<int> flow(g);
    SmartDigraph::Node s, t;

  try {
        digraphReader(g, "digraph.lgf"). // read the directed graph into g
        arcMap("capacity", cap).       // read the 'capacity' arc map into cap
        arcMap("cost", cost).          // read 'cost' for for arcs
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

    CapacityScaling<SmartDigraph> cs(g);

    // First run
    cs.upperMap(cap).costMap(cost).stSupply(s, t, 100).run();

    std::cout << "Total cost: " << cs.totalCost<double>() << std::endl;

    std::cout << "We can write it to the standard output:" << std::endl;

    cs.flowMap(flow);

    digraphWriter(g).                  // write g to the standard output
        arcMap("capacity", cap).       // write cap into 'capacity'
        arcMap("cost", cost).          // write 'cost' for for arcs
        arcMap("flow", flow).          // write 'flow' for for arcs
        node("source", s).             // write s to 'source'
        node("target", t).             // write t to 'target'
        run();

    return 0;
}


int mainAlt() {
	return 0;
}
