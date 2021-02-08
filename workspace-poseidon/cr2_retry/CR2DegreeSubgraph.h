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

namespace cr2 {

    template <class T>
    class CR2Subgraph {
    protected:
        T* vid_range[2];
        cr2::EDGE* edge_range[2];

        // These members are the index points where the range of a cluster starts and ends.
        // For instance, when certain cluster size is given as 20,
        // the vid_range[START] will point to the index of 0 
        // and vid_range[END] will point to the index of 19.
        // 
        // Another CR2Cluster object's vid_range member will hold 20 and 39.
        // 

    public:
        CR2Subgraph() { 

            // Initialize with all zeros
            vid_range[cr2::LOC::START] = vid_range[cr2::LOC::END] = nullptr;
            edge_range[cr2::LOC::START] = edge_range[cr2::LOC::END] = nullptr;
        };
        virtual ~CR2Subgraph() {};

        // Interface
        // Getter
        virtual T* getVidRangeStart() { return vid_range[cr2::LOC::START]; };
        virtual T* getVidRangeEnd() { return vid_range[cr2::LOC::END]; };

        virtual cr2::EDGE* getEdgeRangeStart() { return edge_range[cr2::LOC::START]; };
        virtual cr2::EDGE* getEdgeRangeEnd() { return edge_range[cr2::LOC::END]; };

        virtual void setVidRangeStart(const T argSt) { vid_range[cr2::LOC::START] = argSt; }; 
        virtual void setVidRangeEnd(const T argEd) { vid_range[cr2::LOC::END] = argEd; };
        

    };

    //
    // Children has an additional member, degree.
    template <class T>
    class CR2DegreeSubgraph final : public CR2Subgraph<T> {
    private:
        // log_degree stores the value 0 to 5.
        cr2::DEGREE log_degree;
        uint32_t num_virtual_nodes[2]; // INCOMING(0), OUTGOING(1)

    public:
        CR2DegreeSubgraph(cr2::DEGREE argdeg) 
            : log_degree(argdeg) { 
                num_virtual_nodes[INCOMING] = num_virtual_nodes[OUTGOING] = 0;
            };
        virtual ~CR2DegreeSubgraph() { };

        // Interface
        unsigned getDegree() { return static_cast<unsigned>(degree); };
        void setDegree(cr2::DEGREE argDeg) { degree = argDeg; };

        uint32_t& accessNumVirtualNodes(int argDirection) { return num_virtual_nodes[argDirection]; };
    };
}

#endif


