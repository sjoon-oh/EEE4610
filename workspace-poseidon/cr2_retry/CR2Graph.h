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

    // CR2Graph class is just a container.
    // 
    class CR2Graph final {
        private:
            // Meta data
            uint32_t num_nodes = 0; // number of nodes
            uint32_t num_intra_cluster = 0; // number of intra cluster

            // Graph information
            std::vector<CR2Cluster<cr2::DENSE_TYPE>*> intra_cluster;
            cr2::CR2Cluster<cr2::SPARSE_TYPE>* inter_cluster; // Fixed to have only one.

            // Alt
        public:
            // ctor & dtor
            CR2Graph() = default;
            ~CR2Graph() {};

            // Interface
            unsigned doRegisterCluster(uint32_t, uint32_t);
            unsigned doRegisterEdge(const std::vector<cr2::Edge>&);

            unsigned doDestroy();


#ifdef CONSOLE_OUT_ENABLE // Debugging purpose
            void console_out_object_info();
#endif
    };
};

#endif