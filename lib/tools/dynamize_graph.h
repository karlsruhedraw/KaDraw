//
// Author: Christian Schulz <christian.schulz@kit.edu>
// 

#ifndef DYNAMIZE_GRAPH_BZNTNBBI
#define DYNAMIZE_GRAPH_BZNTNBBI

#include "config.h"
#include "data_structure/graph_access.h"

class dynamize_graph {
public:
        dynamize_graph();
        virtual ~dynamize_graph();


        void dynamize_the_graph( Config & config, graph_access & input_graph, graph_access & output_graph);
private:
        void block_edges( Config & config, graph_access & G, std::vector< bool > & keep_edge);

};


#endif /* end of include guard: DYNAMIZE_GRAPH_BZNTNBBI */
