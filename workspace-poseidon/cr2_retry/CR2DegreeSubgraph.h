// [Refactoring] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

//

#pragma once
#include "CR2.h"

#ifdef CR2

#include "CR2Config.h"
#include <iostream>

#include <assert.h>
#include <math.h>

#include "CR2Type.h"
#include "CR2Cluster.h"

namespace cr2 {

    class CR2Subgraph {
    protected:
        cr2::NODE node_range[2][2] = {{0, 0}, {0, 0}};
        cr2::EDGE edge_range[2][2] = {{0, 0}, {0, 0}};

        // These members are the index points where the range of a cluster starts and ends.
        // For instance, when certain cluster size is given as 20,
        // the range[START] will point to the index of 0 
        // and range[END] will point to the index of 19.
        // 
        // Another CR2Cluster object's range member will hold 20 and 39.
        // 

    public:
        CR2Subgraph() = default;
        virtual ~CR2Subgraph() {};

        // Interface
        // Getter
        virtual cr2::NODE getNodeRangeStart(unsigned argDir) { return node_range[argDir][cr2::LOC::START]; };
        virtual cr2::NODE getNodeRangeEnd(unsigned argDir) { return node_range[argDir][cr2::LOC::END]; };

        virtual cr2::EDGE getEdgeRangeStart(unsigned argDir) { return edge_range[argDir][cr2::LOC::START]; };
        virtual cr2::EDGE getEdgeRangeEnd(unsigned argDir) { return edge_range[argDir][cr2::LOC::END]; };

        virtual void setNodeRangeStart(unsigned argDir, const cr2::NODE argSt) { node_range[argDir][cr2::LOC::START] = argSt; }; 
        virtual void setNodeRangeEnd(unsigned argDir, const cr2::NODE argEd) { node_range[argDir][cr2::LOC::END] = argEd; };
        

    };

    //
    // Children has an additional member, degree.
    template <class T>
    class CR2DegreeSubgraph final : public CR2Subgraph {
    private:
        // log_degree stores the value 0 to 5.
        cr2::DEGREE log_degree = cr2::DEGREE::DEG_1;
        uint32_t num_virtual_nodes[2] = {0, 0}; // INCOMING(0), OUTGOING(1)
        
        //template <class T>
        friend unsigned cr2::CR2Cluster<T>::doRegisterDegreeSubgraphs(); // need to access num_virtual_nodes

    public:
        CR2DegreeSubgraph(cr2::DEGREE argdeg) = default;
        virtual ~CR2DegreeSubgraph() { };

        // Interface
        unsigned getDegree() { return static_cast<unsigned>(degree); };
        unsigned getNumVirtualNodes(unsigned argDir) { return num_virtual_nodes[argDir]; };

        void setDegree(cr2::DEGREE argDeg) { degree = argDeg; };

        uint32_t& accessNumVirtualNodes(int) = delete; // deprecated
    };
}

#endif


