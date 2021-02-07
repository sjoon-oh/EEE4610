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
        uint32_t id;
        std::vector<cr2::CR2IntraCluster*> intra_cluster_list;

    public:
        CR2Community() : id(0) { };

    };

    class CR2Graph final {
        private:
            uint32_t num_intra_cluster; // number of intra cluster
            std::vector<CR2Community*> community_list;
            cr2::CR2InterCluster* inter_cluster; // Fixed to have only one.



        public:
            // ctor & dtor
            CR2Graph() : num_intra_cluster(0) {};
            CR2Graph(uint32_t argNumIC) : num_intra_cluster(argNumIC) {};
            ~CR2Graph() {};

            // Interface
            unsigned doRegisterIntraCluster(cr2::CR2IntraCluster*);
            unsigned doRegisterInterCluster();

            unsigned setCommunityList();


#ifdef CONSOLE_OUT_ENABLE // Debugging purpose
            void console_out_object_info() {

                printf("[cr2::CR2Graph]\n");
                printf("    num_intra_cluster: %d", num_intra_cluster);
                printf("    intra_cluster_list (size): %d", intra_cluster_list.size());
            }
#endif
    }
}

#endif