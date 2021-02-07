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

    public:
        CR2Cluster() { 
            // Initialize with all zeros
            vid_range[cr2::LOC::HEAD] = vid_range[TAIL] = nullptr;
            edge_range[HEAD] = edge_range[TAIL] = nullptr;            
        };
        virtual ~CR2Clustetr() {};
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

    };

    class CR2InterCluster final : public CR2Cluster<SPARSE_TYPE> {
    public:
        CR2InterCluster(cr2::DEGREE argdeg) : degree(argdeg) { };
        virtual~CR2IntraCluster() { };
    };
}

#endif


