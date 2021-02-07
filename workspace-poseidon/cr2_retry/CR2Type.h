// [Refactoring] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//

//

#include <stdint.h>

namespace cr2 {

    typedef uint16_t DENSE_TYPE;
    typedef uint32_t SPARSE_TYPE;

    typedef uint32_t NODE; // Node type
    typedef uint32_t EDGE; // Edge type

    class Edge final {
        unsigned src_vertex;
        unsigned dst_vertex;
    public:
        Edge(unsigned argsrc, unsigned argdst) : src_vertex(argsrc), dst_vertex(argdst) { };

        // Getter
        const unsigned getSrcVertex() { return src_vertex; };
        const unsigned getDstVertex() { return dst_vertex; };
    };

    // Specifies degree
    enum DEGREE {
        DEG_1 = 1, 
        DEG_2 = 2, 
        DEG_4 = 4, 
        DEG_8 = 8, 
        DEG_16 = 16, 
        DEG_32 = 32
    };

    // Cluster ID
    inline DENSE_TYPE cluster_id(SPARSE_TYPE argVid) { return argVid & 0xFFFF0000; }

    enum LOC { HEAD = 0, TAIL};
}