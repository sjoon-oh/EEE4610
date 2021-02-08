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
    class CR2Cluster {
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
        CR2Cluster() { 
            
            // Initialize with all zeros
            vid_range[cr2::LOC::START] = vid_range[cr2::LOC::END] = nullptr;
            edge_range[cr2::LOC::START] = edge_range[cr2::LOC::END] = nullptr;
        };
        virtual ~CR2Cluster() {};

        // Interface
        virtual T* getVidRangeStart() { return vid_range[cr2::LOC::START]; };
        virtual T* getVidRangeEnd() { return vid_range[cr2::LOC::END]; };

        virtual cr2::EDGE* getEdgeRangeStart() { return edge_range[cr2::LOC::START]; };
        virtual cr2::EDGE* getEdgeRangeEnd() { return edge_range[cr2::LOC::END]; };
        

    };


    class CR2IntraCluster final : public CR2Cluster<DENSE_TYPE> {
    private:
        unsigned cluster_id;
        cr2::DEGREE degree;

    public:
        CR2IntraCluster(unsigned argCid, cr2::DEGREE argdeg) : cluster_id(argCid), degree(argdeg) { };
        virtual ~CR2IntraCluster() { };

        // Interface
        unsigned getClusterId() { return cluster_id; };
        unsigned getDegree() { return static_cast<unsigned>(degree); };

        void setClusterId(unsigned argCid) { cluster_id = argCid; };
        void setDegree(cr2::DEGREE argDeg) { degree = argDeg; };

    };

    class CR2InterCluster final : public CR2Cluster<SPARSE_TYPE> {
    public:
        CR2InterCluster() { };
        virtual ~CR2InterCluster() { };
    };
}

#endif


