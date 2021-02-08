// [Refactoring] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

//

#pragma once
#include "CR2Config.h"

#ifdef CR2

#include <cstdio>

#include "CR2Type.h"
#include "CR2DegreeSubgraph.h"

#include "./util/platform_atomics.h"


namespace cr2 {

    // Community class
    template <class T>
    class CR2Cluster final {
    private:
        // Meta data
        uint32_t id;
        uint32_t num_edges;
        uint32_t num_virtual_nodes[2]; // Number of total virtual nodes

        // Graph data
        uint32_t* vertex_degrees[2]; // IN(0), OUT(1)
        cr2::CR2DegreeSubgraph<T> degree_subgraph[6]; 
            // DEG_1(0), DEG_2(1), DEG_4(2), DEG_8(3), DEG_16(4), DEG_32(5)

        // handle
        // std::vector<cr2::CR2IntraCluster*> intra_cluster_list;
        T* vertex_id_list[2];

    public:
        // ctor & dtor
        CR2Cluster(uint32_t, uint32_t, uint32_t);
        ~CR2Cluster();

        // Interface
        // Call in a sequence. 
        // 1. doRegisterSingleDegree
        // 2. doRegisterDegreeSubgraphs
        // 3. doBuildVertexIDList
        unsigned doRegisterSingleDegree(uint32_t, uint32_t);
        unsigned doRegisterDegreeSubgraphs();

        unsigned doBuildVertexIDList();

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
        degree_subgraph[log_deg] = nullptr; // gnd

    // allocate using the size
    uint32_t num_nodes = argTail - argHead;

    // Each index value represents a vertex id
    // Initialize arrays with all zeros.
    this->vertex_degrees[IN] = new uint32_t[num_nodes];
    this->vertex_degrees[OUT] = new uint32_t[num_nodes];

    for (int vertex_id = 0; vertex_id < num_nodes; vertex_id++)
        vertex_degrees[IN][vertex_id] = vertex_degrees[OUT][vertex_id] = 0;

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

template <class T>
unsigned cr2::CR2Cluster<T>::doRegisterDegreeSubgraphs() {

    for (int log_deg = 0; log_deg < 6; log_deg++)
        degree_subgraph[log_deg].setDegree(log_deg);

    // Prepare the degreed subgraph.
    // There are six Degreed subgraphs for a single cluster(community). 
    #pragma omp parallel for
    for (int vertex_id = 0; vertex_id < num_nodes; vertex_id++) {

        // Temporary values!!
        // First option: IN(0), OUT(1)
        // Second option: DEG_1(0), DEG_2(1), DEG_4(2), DEG_8(3), DEG_16(4), DEG_32(5)
        uint32_t num_nodes[2][6];
        uint32_t num_remains[2][6];

        num_nodes[IN][cr2::DEG_32] = vertex_degrees[IN][vertex_id] / 32; // Number of 32-degreed vertex
        num_remains[IN][cr2::DEG_32] = vertex_degrees[IN][vertex_id] % 32; 

        num_nodes[OUT][cr2::DEG_32] = vertex_degrees[OUT][vertex_id] / 32;
        num_remains[OUT][cr2::DEG_32] = vertex_degrees[OUT][vertex_id] % 32;

        for (unsigned log_deg = cr2::DEG_16; cr2::DEG_1 <= log_deg; log_deg--) {
            // Keep dividing and store remains to propagate to the next degree-array.

            num_nodes[IN][log_deg] = num_remains[IN][log_deg + 1] / uint32_t(pow(2, log_deg));
            num_nodes[IN][log_deg] = num_remains[IN][log_deg + 1] % uint32_t(pow(2, log_deg));
            num_nodes[OUT][log_deg] = num_remains[OUT][log_deg + 1] / uint32_t(pow(2, log_deg));
            num_nodes[OUT][log_deg] = num_remains[OUT][log_deg + 1] % uint32_t(pow(2, log_deg));
        } // So far, we have recorded the number of size of DegreeSubgraph.
        
        // Store the number of virtual nodes to each DegreeSubgraph objects.
        for (unsigned log_deg = cr2::DEG_1; log_deg <= cr2::DEG_32; log_deg++) {

            if (num_nodes[IN][log_deg] != 0)
                fetch_and_add(
                    degree_subgraph->accessNumVirtualNodes(IN), num_nodes[IN][log_deg] );

            if (num_nodes[OUT][log_deg] != 0)
                fetch_and_add(
                    degree_subgraph->accessNumVirtualNodes(OUT), num_nodes[OUT][log_deg] );
        }
    }
    
    //
    // Refactored - Originally countNumOfTotalEdges and countNumOfTotalVirtualNodes from PartialGraph.
    for (unsigned log_deg = cr2::DEG_1; log_deg <= cr2::DEG_32; log_deg++) {
        this->num_edges += (degree_subgraph->accessNumVirtualNodes(IN) * uint32_t(pow(2, log_deg)));
        this->num_virtual_nodes[IN] += degree_subgraph->accessNumVirtualNodes(IN);
        this->num_virtual_nodes[OUT] += degree_subgraph->accessNumVirtualNodes(OUT);
    }

    return 0;
}


template <class T>
unsigned cr2::CR2Cluster<T>::doBuildVertexIDList() { // Third step!!

    this->vertex_id_list[IN] = new T[num_edges];
    this->vertex_id_list[OUT] = new T[OUT];






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

