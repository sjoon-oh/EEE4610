// [Refactoring] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

//

#pragma once
#include "CR2Config.h"

#ifdef CR2

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "CR2Type.h"
#include "CR2DegreeSubgraph.h"

#include "./util/platform_atomics.h"

#define POW2(X) (uint32_t(pow(2, X)))

namespace cr2 {

    // Community class
    template <class T>
    class CR2Cluster final {
    private:
        // Meta data
        uint32_t id = 0;
        uint32_t num_nodes = 0;
        uint32_t num_edges = 0;
        uint32_t num_virtual_nodes[2] = {0, 0}; // Number of total virtual nodes

        // Graph data
        uint32_t* vertex_degrees[2] = { nullptr, nullptr }; // IN(0), OUT(1)
        cr2::CR2DegreeSubgraph<T> degree_subgraph[6];
            // DEG_1(0), DEG_2(1), DEG_4(2), DEG_8(3), DEG_16(4), DEG_32(5)

        // handle
        // std::vector<cr2::CR2IntraCluster*> intra_cluster_list;
        T* vertex_id_list[2];

    public:
        // ctor & dtor
        CR2Cluster() = default;
        CR2Cluster(uint32_t, uint32_t, uint32_t);
        ~CR2Cluster();

        // Interface
        // Call in a sequence. 
        // 1. doRegisterSingleDegree
        // 2. doRegisterDegreeSubgraphs
        // 3. doVertexSplit
        unsigned doRegisterSingleDegree(uint32_t, uint32_t);
        unsigned doRegisterDegreeSubgraphs();
        unsigned doVertexSplit();

#ifdef CONSOLE_OUT_ENABLE // Debugging purpose
        void console_out_object_info();
#endif
    };
};


// Methods definition starts from here.
// ctor
template <class T>
cr2::CR2Cluster<T>::CR2Cluster(uint32_t argId, uint32_t argHead, uint32_t argTail) 
    : id(argId), // This is the cluster id (Community id)
    num_edges(0) { 

    // To all zeros!
    num_virtual_nodes[IN] = num_virtual_nodes[OUT] = 0;
    vertex_id_list[IN] = vertex_id_list[OUT] = nullptr;

    for (int log_deg = 0; log_deg < 6; log_deg++)
        degree_subgraph[log_deg].setDegree(log_deg);

    // allocate using the size
    num_nodes = argTail - argHead;

    // Each index value represents a vertex id
    // Initialize arrays with all zeros.
    this->vertex_degrees[IN] = new uint32_t[num_nodes]();
    this->vertex_degrees[OUT] = new uint32_t[num_nodes]();

    // for (int vertex_id = 0; vertex_id < num_nodes; vertex_id++)
    //     vertex_degrees[IN][vertex_id] = vertex_degrees[OUT][vertex_id] = 0;

};

template <class T>
cr2::CR2Cluster<T>::~CR2Cluster() {

    delete[] this->vertex_degrees[IN];
    delete[] this->vertex_degrees[OUT];

    if (vertex_id_list[IN] != nullptr) delete[] vertex_id_list[IN];
    if (vertex_id_list[OUT] != nullptr) delete[] vertex_id_list[OUT];
};


// Interface
// To fully function, the api should be called in a sequence.
// 1. Call doRegisterSingleDegree in iteration to count vertices' incoming/outgoing degrees.
// 2. Then call doArrangeCommunity to generate metadata for the community

//
// Registers single degree. It receives single edge, (src, dst) for its argument.
template <class T>
unsigned cr2::CR2Cluster<T>::doRegisterSingleDegree(uint32_t argSrc, uint32_t argDst) {

    fetch_and_add(in_degree[argDst], 1);
    fetch_and_add(out_degree[argSrc], 1);

    return 0;
};


//
// Generates DegreeSubgraph information. Example: Size, offset etc.
template <class T>
unsigned cr2::CR2Cluster<T>::doRegisterDegreeSubgraphs() {

    //
    // 1st Step. Prepare the degreed subgraph.
    // There are six Degreed subgraphs for a single cluster(community). 
    #pragma omp parallel for
    for (int vertex_id = 0; vertex_id < this->num_nodes; vertex_id++) {
        // num_nodes refers to the cluster size; the number of nodes within a cluster.

        // Temporary values!!
        // First option: IN(0), OUT(1)
        // Second option: DEG_1(0), DEG_2(1), DEG_4(2), DEG_8(3), DEG_16(4), DEG_32(5)
        uint32_t num_nodes_of[2][6];
        uint32_t num_remains_of[2][6];

        num_nodes_of[IN][cr2::DEG_32] = vertex_degrees[IN][vertex_id] / 32; // Number of 32-degreed vertex
        num_remains_of[IN][cr2::DEG_32] = vertex_degrees[IN][vertex_id] % 32; 

        num_nodes_of[OUT][cr2::DEG_32] = vertex_degrees[OUT][vertex_id] / 32;
        num_remains_of[OUT][cr2::DEG_32] = vertex_degrees[OUT][vertex_id] % 32;

        for (unsigned log_deg = cr2::DEG_16; cr2::DEG_1 <= log_deg; log_deg--) {
            // Keep dividing and store remains to propagate to the next degree-array.
            
            num_nodes_of[IN][log_deg] = num_remains_of[IN][log_deg + 1] / POW2(log_deg);
            num_nodes_of[IN][log_deg] = num_remains_of[IN][log_deg + 1] % POW2(log_deg);
            
            num_nodes_of[OUT][log_deg] = num_remains_of[OUT][log_deg + 1] / POW2(log_deg);
            num_nodes_of[OUT][log_deg] = num_remains_of[OUT][log_deg + 1] % POW2(log_deg);
        } // So far, we have recorded the number of size of DegreeSubgraph.
        
        // This is the key point of all these shits.
        // Record the number of virtual nodes to each DegreeSubgraph objects.
        for (unsigned log_deg = cr2::DEG_1; log_deg <= cr2::DEG_32; log_deg++) {

            if (num_nodes_of[IN][log_deg] != 0)
                fetch_and_add(
                    degree_subgraph[log_deg].num_virtual_nodes[IN], num_nodes[IN][log_deg] );

            if (num_nodes_of[OUT][log_deg] != 0)
                fetch_and_add(
                    degree_subgraph[log_deg].num_virtual_nodes[OUT], num_nodes[OUT][log_deg] );
        }
    }

    //
    // 2nd Step.
    { // Originally, PartialGraph::calculatePrefixSum()
        uint32_t node_current[2] = { 0, };
        uint32_t edge_current[2] = { 0, };

        // Record the starting point of each degree-subgraphs.
        // Recall that the structures share the same single target array: 
        //     vertex_id_list - The ultimate purpose of all these shits.
        for (unsigned log_deg = cr2::DEG_1; log_deg <= cr2::DEG_32; log_deg++) {

            degree_subgraph[log_deg].setNodeRangeStart(IN, node_current[IN]); // former prefix sum
            degree_subgraph[log_deg].setEdgeRangeStart(IN, edge_current[IN]);

            degree_subgraph[log_deg].setNodeRangeStart(OUT, node_current[OUT]);
            degree_subgraph[log_deg].setEdgeRangeStart(OUT, edge_current[OUT]);

            node_current[IN] += degree_subgraph[log_deg].getNumVirtualNodes(IN);
            edge_current[IN] += 
                (degree_subgraph[log_deg].getNumVirtualNodes(IN) * POW2(log_deg));
            
            node_current[OUT] += degree_subgraph[log_deg].getNumVirtualNodes(OUT);
            edge_current[OUT] += 
                (degree_subgraph[log_deg].getNumVirtualNodes(OUT) * POW2(log_deg));
        }
    } // PartialGraph::calculatePrefixSum() done.
    
    // Bonus Step. Record additional information such as number of virtual nodes etc.
    // Refactored - Originally countNumOfTotalEdges and countNumOfTotalVirtualNodes from PartialGraph.
    for (unsigned log_deg = cr2::DEG_1; log_deg <= cr2::DEG_32; log_deg++) {

        this->num_edges 
            += (degree_subgraph->num_virtual_nodes[IN] * POW2(log_deg));

        this->num_virtual_nodes[IN] += degree_subgraph->accessNumVirtualNodes(IN);
        this->num_virtual_nodes[OUT] += degree_subgraph->accessNumVirtualNodes(OUT);

        // Note that num_virtual_nodes from CR2DegreeSubgraph refers to 
        // the number belongs to each CR2DegreeSubgraph object,
        // and the num_virtual_nodes from CR2Cluster refers to 
        // the number of total virtual nodes in the CR2Clusrer.
    }

    return 0;
}


//
// Refactored version of PartialGraph::splitNodes(). 
template <class T>
unsigned cr2::CR2Cluster<T>::doVertexSplit() { // Third step!!

    // Prepare to fight!
    this->vertex_id_list[IN] = new T[num_edges];
    this->vertex_id_list[OUT] = new T[OUT];

    // Originally, PartialGraph::buildVertexIDList_in() or _out()
    // Outer Loop: Traverse through vertex ID
    for (int vertex_id = 0; vertex_id < num_nodes; vertex_id++) {
        
        // Inner Loop 1: Is incoming edge or the outcoming edge?
        for (int edge_direction = cr2::IN;
            edge_direction <= cr2::OUT; edge_direction++) {

            if (vertex_degrees[edge_direction][vertex_id] != 0) { 
                uint32_t current_remain = vertex_degrees[edge_direction][vertex_id];

                // Inner Loop 2: Traverse through 2^DEG_32(5) to 2^DEG_1(1)
                for (unsigned log_deg = cr2::DEG_32; log_deg >= cr2::DEG_1; log_deg++) {
                    uint32_t degree = POW2(log_deg);
                    uint32_t num_virtual_nodes_s = current_remain / degree;


                    for (uint32_t pos = degree_subgraph[log_deg].getNodeRangeStart(edge_direction);
                        pos < degree_subgraph[log_deg].getNodeRangeStart(edge_direction) + 
                    )







                    

                }
            }
        }
    }
    





    return 0;
}




#ifdef CONSOLE_OUT_ENABLE // Debugging purpose
template <class T>
void cr2::CR2Cluster<T>::console_out_object_info() {

    printf("[cr2::CR2Cluster]\n");
    printf("    id: %d\n", id);
    printf("    community size (node size): %d\n", vid_range[cr2::LOC::END] - vid_range[cr2::LOC::START]);

}
#endif
#endif

