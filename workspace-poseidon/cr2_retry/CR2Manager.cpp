// [Refactoring] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

#include "CR2Manager.h"
#include "./util/platform_atomics.h"

#include <algorithm>

//
// Finds the maximum vertex id, which becomes the (total number of edges - 1).
// arg: const std::vector<cr2::Edge>&
unsigned cr2::CR2Manager::altCountNodes(const std::vector<cr2::Edge>& argEdgeList) {
    unsigned num_nodes = 0;

    #pragma omp parallel for reduction(max : num_nodes)
    for (auto& it: argEdgeList) {
        num_nodes = std::max(num_nodes, it->getSrcVertex());
        num_nodes = std::max(num_nodes, it->getDstVertex());
    }

    return 0;
};



//
// Generates GR2Graph and returns the graph.
// arg: const std::vector<cr2::Edge>&
// return: cr2::CR2Graph*
cr2::CR2Graph* cr2::CR2Manager::doBuild(const std::vector<cr2::Edge>& argEdgeList) {

    if (this->cr2_graph != nullptr) doReset(); // unlink the existing graph
    this->cr2_graph  = new cr2::CR2Graph(); // New dummy
    
    // Finding the maximum vertex id of the input graph
    num_original_nodes = this->altCountNodes(argEdgeList) + 1;
    num_edges = argEdgeList.size(); // Record the number of edges (original input graph)

    



};


unsigned cr2::CR2Manager::doReset() {

    if (cr2_graph != nullptr) delete cr2_graph;
    cr2_graph = nullptr;

    return 0;
}