// [Refactoring] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

//

#pragma once
#include "CR2Config.h"

#ifdef CR2
#include "CR2Type.h"
#include "CR2Graph.h"

#define POW(a, b) ((uint32_t)pow(a, b))

namespace cr2 {

    // CR2Manager class. Manages the CR2Graph.
    class CR2Manager final {
        private:
            cr2::CR2Graph* cr2_graph;
            
            unsigned num_original_nodes; // Number of original graph's nodes
            unsigned num_edges;
            unsigned num_virtual_nodes; // Total number of virtual nodes
            unsigned num_community;

            // Core
            unsigned altCountNodes(const std::vector<cr2::Edge>&); 
            unsigned altCountVirtualNodes();


        public:
            // ctor & dtor
            CR2Manager() : 
                num_original_nodes(0), 
                num_edges(0), 
                num_virtual_nodes(0),
                num_community(0) { 
                };
            ~CR2Manager() { this->doReset(); };

            // Interface
            cr2::CR2Graph* doBuild(const std::vector<cr2::Edge>&);
            // unsigned doRegister();
            unsigned doReset();

            // Getter
            const cr2::CR2Graph* getCr2Graph() { return cr2_graph; };
            const unsigned getNumVirtualNodes() { return num_virtual_nodes; };

#ifdef CONSOLE_OUT_ENABLE // Debugging purpose
            void console_out_object_info() {

                printf("[cr2::CR2Manager]\n");
                printf("    cr2_graph: %p", cr2_graph);
            }
#endif
    }
}

#endif
