//
// Author: Christian Schulz <christian.schulz@kit.edu>
// 

#ifndef SHORTEST_PATHS_HQGDJB3G
#define SHORTEST_PATHS_HQGDJB3G

#include <vector>
#include "data_structure/graph_access.h"

class shortest_paths {
public:
        shortest_paths();
        virtual ~shortest_paths();

        void one_to_many_unit_weight( graph_access & G, NodeID & source, std::vector< int > & distances);
};


#endif /* end of include guard: SHORTEST_PATHS_HQGDJB3G */
