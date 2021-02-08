// [Refactoring] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

#include "CR2Cluster.h"
#include "CR2Graph.h"

#include "./util/platform_atomics.h"

// Defined Utility
#define COMMUNITY_ID(X) (X / COMMUNITY_SIZE)
#define COMMINUTY_INNER_IDX(X) (X % COMMUNITY_SIZE)

#define IS_SAME_CLUSTER(X, Y) (( (X / COMMUNITY_SIZE) == (Y / COMMUNITY_SIZE) )) // query

//
// Registers single degree. It receives single edge, (src, dst) for its argument.
unsigned cr2::CR2Community::doRegisterSingleDegree(uint32_t argSrc, uint32_t argDst) {

    fetch_and_add(incoming_degree[argDst], 1);
    fetch_and_add(outgoing_degree[argSrc], 1);

    return 0;
};








//
// This function plays a role of the existing DenseGraph's (and SparseGraph's) constructor.
// Calculates the number of community, counting starting index and the back index of the array as divider.
// Be sure to call this function before calling the doRegisterEdge() function.
unsigned cr2::CR2Graph::doRegisterCommunity(uint32_t argNumCommunity, uint32_t argNumNodes) {

    // Record the fixed sized range to CR2Community, 
    // this goes for the community that holds intra-clusters.
    for (uint32_t i = 0; i < argNumCommunity; i++) {
        
        uint32_t vertex_st = i * COMMUNITY_SIZE; // Next range
        uint32_t vertex_ed = (i == intra_cluster_community.size() - 1) ? 
            argNumNodes : (i + 1) * COMMUNITY_SIZE;

        // By having the size of COMMUNITY_SIZE, register the new CR2Community.
        // Number of nodes are calculated within the ctor of CR2Community object.
        intra_cluster_community.push_back(new CR2Community(i, vertex_st, vertex_ed));
    }

    // Now, for the inter-cluster community.
    // Since the inter-cluster is one and only, there is no need for complicated operations.
    // The arguments are: 0 for id, 0 for starting index, and the argNumNodes - 1 for last index.
    inter_cluster_community = new CR2Community(0, 0, argNumNodes - 1);

    this->num_nodes = argNumCommunity; // Register member
    return 0;
};


//
// This function generates edge list into an empty graph.
unsigned cr2::CR2Graph::doRegisterEdge(const std::vector<cr2::Edge>& argEdgeList) {
    
    // First step!
    // Iterates the edge, and count the degree for each vertex. 
    // How many incoming and outgoing edges are there for a vertex?
    for (auto it: argEdgeList) {

        cr2::EDGE src = it.getSrcVertex();
        cr2::EDGE dst = it.getDstVertex();
        
        if (IS_SAME_CLUSTER(src, dst)) // In case if it belongs to same cluster,
            this->intra_cluster_community[COMMUNITY_ID(src)]->doRegisterSingleDegree(
                COMMINUTY_INNER_IDX(src), COMMINUTY_INNER_IDX(dst)
            ); // At its cluster (with its appropriate community id), should edges be stored.

        else // If it is not the case (not belongs to the same cluster)
            this->inter_cluster_community->doRegisterSingleDegree(
                COMMINUTY_INNER_IDX(src), COMMINUTY_INNER_IDX(dst)
            );
    }

    // so far, refactored former CommCSRBuilder::countDegrees



    return 0;
};


//
// Releases all the memory held in this container.
unsigned cr2::CR2Graph::doDestroy() {

    if (intra_cluster_community.size() != 0) // If there exists any community
        for (auto it: intra_cluster_community) delete it; // free

    delete inter_cluster_community;
    return 0;
};




#ifdef CONSOLE_OUT_ENABLE // Debugging purpose
void cr2::CR2Community::console_out_object_info() {

    printf("[cr2::CR2Community]\n");
    printf("    id: %d\n", id);
    printf("    community size (node size): %d\n", vid_range[cr2::LOC::END] - vid_range[cr2::LOC::START]);
    printf("    vid_range[START]: %d\n", vid_range[cr2::LOC::START]);
    printf("    vid_range[END]: %d\n", vid_range[cr2::LOC::END]);

}

void cr2::CR2Graph::console_out_object_info() {

    printf("[cr2::CR2Graph]\n");
    printf("    num_intra_cluster: %d\n", num_intra_cluster);
    printf("    intra_cluster_community (size): %d\n", intra_cluster_community.size());
}
#endif