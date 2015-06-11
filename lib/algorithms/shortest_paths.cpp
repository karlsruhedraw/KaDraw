//
// Author: Christian Schulz <christian.schulz@kit.edu>
// 

#include "shortest_paths.h"
#include "data_structure/priority_queues/minNodeHeap.h"

shortest_paths::shortest_paths() {
                
}

shortest_paths::~shortest_paths() {
                
}

void shortest_paths::one_to_many_unit_weight( graph_access & G, NodeID & source, std::vector< int > & distances) {
        // perform a BFS to compute the distance from source to all other vertices
        distances.resize( G.number_of_nodes() ); 
        std::queue< NodeID > bfsqueue;

        forall_nodes(G, node) {
                distances[node] = -1;
        } endfor

        distances[source] = 0;
        int cur_deepth    = 0;

        bfsqueue.push(source);
        while(!bfsqueue.empty()) {
                NodeID node = bfsqueue.front();
                bfsqueue.pop(); 

                if (distances[node] == cur_deepth) {
                        cur_deepth++;
                }

                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        if(distances[target] == -1) {
                                distances[target] = cur_deepth;        
                                bfsqueue.push(target);
                        }
                } endfor
        }

}
