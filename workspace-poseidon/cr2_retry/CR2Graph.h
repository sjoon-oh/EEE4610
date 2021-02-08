// [Refactoring] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

//

#pragma once
#include "CR2Config.h"

#ifdef CR2

#include <vector>
#include <cstdio>

#include "CR2Type.h"
#include "CR2Cluster.h"

namespace cr2 {

    // Community class
    class CR2Community final {
    private:
        // Meta data
        uint32_t id;
        uint32_t vid_range[2];

        uint32_t* incoming_degree;
        uint32_t* outgoing_degree;

        // handle
        std::vector<cr2::CR2IntraCluster*> intra_cluster_list;


    public:
        // ctor & dtor
        CR2Community() : id(0) { };
        CR2Community(uint32_t argId, uint32_t argHead, uint32_t argTail) 
            : id(argId) { 
                // HEAD and TAIL points to the range index. 
                vid_range[cr2::LOC::START] = argHead;
                vid_range[cr2::LOC::END] = argTail;

                // allocate using the size
                uint32_t num_nodes = argTail - argHead;

                // Each index value represents a vertex id
                this->incoming_degree = new uint32_t[num_nodes];
                this->outgoing_degree = new uint32_t[num_nodes];
            };

        ~CR2Community() {
            delete[] incoming_degree;
            delete[] outgoing_degree;
        }

        unsigned doRegisterIntraCluster(); // push single intra-cluster
        unsigned doRegisterSingleDegree();

#ifdef CONSOLE_OUT_ENABLE // Debugging purpose
        void console_out_object_info();
#endif
    };



    // CR2Graph class is just a container.
    // 
    class CR2Graph final {
        private:
            // Meta data
            uint32_t num_nodes; // number of nodes
            uint32_t num_intra_cluster; // number of intra cluster

            // Graph information
            std::vector<CR2Community*> intra_cluster_community;
            cr2::CR2Community* inter_cluster_community; // Fixed to have only one.

            // Alt


        public:
            // ctor & dtor
            CR2Graph() 
                : num_intra_cluster(0), 
                inter_cluster_community(nullptr)
                {

                };
            ~CR2Graph() {};

            // Interface
            unsigned doRegisterCommunity(uint32_t, uint32_t);
            unsigned doUnregisterCommunity();

            unsigned doRegisterEdge(const std::vector<cr2::Edge>&);

            unsigned doDestroy();


#ifdef CONSOLE_OUT_ENABLE // Debugging purpose
            void console_out_object_info();
#endif
    };
};

#endif