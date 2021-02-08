// [Refactoring] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

#include "CR2Cluster.h"
#include "CR2Graph.h"

#include "./util/platform_atomics.h"

// Defined Utility
#define CLUSTER_ID(X) (X / CLUSTER_SIZE)
#define COMMINUTY_INNER_IDX(X) (X % CLUSTER_SIZE)

#define IS_SAME_CLUSTER(X, Y) (( (X / CLUSTER_SIZE) == (Y / CLUSTER_SIZE) )) // query

//
// This function plays a role of the existing DenseGraph's (and SparseGraph's) constructor.
// Calculates the number of community, counting starting index and the back index of the array as divider.
// Be sure to call this function before calling the doRegisterEdge() function.
unsigned cr2::CR2Graph::doRegisterCluster(uint32_t argNumCommunity, uint32_t argNumNodes) {

    // Record the fixed sized range to CR2Cluster, 
    // this goes for the community that holds intra-clusters.
    for (uint32_t i = 0; i < argNumCommunity; i++) {
        
        uint32_t vertex_st = i * CLUSTER_SIZE; // Next range
        uint32_t vertex_ed = (i == intra_cluster.size() - 1) ? 
            argNumNodes : (i + 1) * CLUSTER_SIZE;

        // By having the size of CLUSTER_SIZE, register the new CR2Cluster.
        // Number of nodes are calculated within the ctor of CR2Cluster object.
        intra_cluster.push_back(
            new cr2::CR2Cluster<cr2::DENSE_TYPE>(i, vertex_st, vertex_ed));
    }

    // Now, for the inter-cluster community.
    // Since the inter-cluster is one and only, there is no need for complicated operations.
    // The arguments are: 0 for id, 0 for starting index, and the argNumNodes - 1 for last index.
    inter_cluster = new cr2::CR2Cluster<cr2::SPARSE_TYPE>(0, 0, argNumNodes - 1);

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
            this->intra_cluster[CLUSTER_ID(src)]->doRegisterSingleDegree(
                COMMINUTY_INNER_IDX(src), COMMINUTY_INNER_IDX(dst)
            ); // At its cluster (with its appropriate community id), should edges be stored.

        else // If it is not the case (not belongs to the same cluster)
            this->inter_cluster->doRegisterSingleDegree(
                COMMINUTY_INNER_IDX(src), COMMINUTY_INNER_IDX(dst)
            );
    } // so far, refactored former CommCSRBuilder::countDegrees







    // so far, refactored former splitNodes()








    // so far, refactored former CommCSRBuilder::buildNeighborList

    return 0;
};


//
// Releases all the memory held in this container.
unsigned cr2::CR2Graph::doDestroy() {

    if (intra_cluster.size() != 0) // If there exists any community
        for (auto it: intra_cluster) delete it; // free

    delete inter_cluster;
    return 0;
};




#ifdef CONSOLE_OUT_ENABLE // Debugging purpose
void cr2::CR2Graph::console_out_object_info() {

    printf("[cr2::CR2Graph]\n");
    printf("    num_intra_cluster: %d\n", num_intra_cluster);
    printf("    intra_cluster_community (size): %d\n", intra_cluster.size());
}
#endif