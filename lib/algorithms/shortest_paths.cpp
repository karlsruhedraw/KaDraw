/******************************************************************************
 * shortest_paths.cpp 
 *
 * Source of KaDraw -- Karlsruhe Graph Drawing 
 ******************************************************************************
 * Copyright (C) 2015 Christian Schulz <christian.schulz@kit.edu>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/


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
