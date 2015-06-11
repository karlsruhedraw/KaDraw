//
// Author: Christian Schulz <christian.schulz@kit.edu>
// 

#include "dynamize_graph.h"
#include "random_functions.h"

dynamize_graph::dynamize_graph() {
                
}

dynamize_graph::~dynamize_graph() {
                
}

void dynamize_graph::block_edges( Config & config, graph_access & G, std::vector< bool > & keep_edge) {
                std::queue<NodeID> node_queue;
                std::vector<int> deepth(G.number_of_nodes(), -1);
                std::vector<NodeID> canditates;
                int cur_deepth = 0;
                NodeID v = random_functions::nextInt(0,G.number_of_nodes()-1);
                node_queue.push(v);
                deepth[v] = 0;
                cur_deepth++;

                /***************************
                 * Do the BFS
                 ***************************/
                while (!node_queue.empty()) {
                        NodeID v = node_queue.front();
                        node_queue.pop();

                        if (deepth[v] == cur_deepth) {
                                cur_deepth++;
                        }
                        forall_out_edges(G,e,v) {
                                NodeID target = G.getEdgeTarget(e);
                                if(deepth[target] == -1) {
                                        keep_edge[e] = true;
                                        //also keep bw edge
                                        forall_out_edges(G, e_bar, target) {
                                                NodeID t_prime = G.getEdgeTarget(e_bar);
                                                if( v == t_prime ) {
                                                        keep_edge[e_bar] = true;
                                                        break;
                                                }
                                        } endfor
                                        deepth[target] = cur_deepth;
                                        node_queue.push(target);
                                        canditates.push_back(target);
                                }
                        } endfor
                }
} 

void dynamize_graph::dynamize_the_graph( Config & config, 
                         graph_access & G, 
                         graph_access & Q) {

        std::vector< std::vector< NodeID > > new_edges(G.number_of_nodes());
        std::vector< bool > keep_edge(G.number_of_edges(), true);
        std::vector< bool > block_edge(G.number_of_edges(), false);
        block_edges( config, G, block_edge);

        int counter = 0; 
        for( unsigned i = 0; i < block_edge.size(); i++) {
                if( block_edge[i] == true ) {
                        counter++;
                }
        }
        std::cout <<  "coutern " <<  counter <<   " " <<   G.number_of_edges()  << std::endl;
        
        int num_edges_to_del = std::min(ceil((config.dyn_percent_edges/100.0) * G.number_of_edges()/2.0), floor((G.number_of_edges()-counter)/2));
        int num_edges_to_ins = num_edges_to_del;
        std::cout <<  "marking edges for deletion"  << std::endl;
        std::cout <<  "num_edges to delete " <<  num_edges_to_del  << std::endl;

        //mark edges for deletion
        for( int i = 0; i < num_edges_to_del; i++) {

                NodeID v = random_functions::nextInt(0,G.number_of_nodes()-1);
                bool all_edges_deleted = true;
                forall_out_edges(G, e, v) {
                        if( keep_edge[e] && !block_edge[e]) {
                                all_edges_deleted = false;
                                break;
                        }
                } endfor

                if( all_edges_deleted ) {
                        i--;
                        continue;
                }

                bool edge_not_found = true;
                while(  edge_not_found ) {
                        EdgeID local_edge = random_functions::nextInt(0,G.getNodeDegree(v)-1);
                        EdgeID e = G.get_first_edge(v) + local_edge;
                        if( !keep_edge[e] || block_edge[e]) {
                                continue;
                        } else {
                                //std::cout <<  "marking edge for removal"  << std::endl;
                                keep_edge[e] = false;

                                // delete bw edge
                                NodeID target = G.getEdgeTarget(e);
                                forall_out_edges(G, e_bar, target) {
                                        NodeID target_bar = G.getEdgeTarget(e_bar);
                                        if(target_bar == v) {
                                                keep_edge[e_bar] = false;
                                        }
                                } endfor
                                edge_not_found = false;
                        
                        }
                }

        }

        std::cout <<  "now inserting new edges"  << std::endl;
        int edge_counter = 0;
        std::cout <<  "edges to insert " <<  num_edges_to_ins  << std::endl;
        for( int i = 0; i < num_edges_to_ins; i++) {
                std::queue<NodeID> node_queue;
                std::vector<int> deepth(G.number_of_nodes(), -1);
                std::vector<NodeID> canditates;
                int cur_deepth = 0;
                NodeID v = random_functions::nextInt(0,G.number_of_nodes()-1);
                node_queue.push(v);
                deepth[v] = 0;
                cur_deepth++;

                // perform first step of BFS explicitly since we do not want newly created edges 
                // at places in which edges already exist
                forall_out_edges(G, e, v) {
                        NodeID target = G.getEdgeTarget(e);
                        deepth[target] = cur_deepth;
                        node_queue.push(target);
                } endfor

                /***************************
                 * Do the BFS
                 ***************************/
                while (!node_queue.empty()) {
                        NodeID v = node_queue.front();
                        node_queue.pop();

                        if (deepth[v] == cur_deepth) {
                                cur_deepth++;
                        }
                        if(cur_deepth > config.dyn_bfs_deepth) {
                                break;
                        }
                        forall_out_edges(G,e,v) {
                                NodeID target = G.getEdgeTarget(e);
                                if(deepth[target] == -1) {
                                        deepth[target] = cur_deepth;
                                        node_queue.push(target);
                                        canditates.push_back(target);
                                }
                        } endfor
                }

                bool already_picked = true;
                while(already_picked && canditates.size() >= 1) {
                        NodeID new_edge_head_pos = random_functions::nextInt(0, canditates.size() - 1);
                        NodeID new_edge_head     = canditates[new_edge_head_pos];
                        already_picked = false;
                        for( int j = 0; j < (int)new_edges[v].size(); j++) {
                                if( new_edges[v][j] == new_edge_head ) {
                                        already_picked = true;
                                        //remove it from the candiates
                                        std::swap( canditates[new_edge_head_pos], canditates[canditates.size()-1]);
                                        canditates.pop_back();
                                        //break;
                                }
                        }

                        if( !already_picked ) {
                                new_edges[v].push_back(new_edge_head);
                                new_edges[new_edge_head].push_back(v);
                                edge_counter++;
                        }                 
                }
                if(canditates.size() == 0) {
                        // no suitable candiate has been found
                        i--;
                }
        }


        // now build the new graph
        Q.start_construction(G.number_of_nodes(), G.number_of_edges());
        forall_nodes(G, node) {
                NodeID v = Q.new_node();
                Q.setNodeWeight(v, G.getNodeWeight(node));
                Q.setCoords(v, G.getX(node), G.getY(node));

                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        if( keep_edge[e] ) {
                                EdgeID e_bar = Q.new_edge(v,target);
                                Q.setEdgeWeight(e_bar, G.getEdgeWeight(e));
                        }
                } endfor
                
                for( int i = 0; i < (int)new_edges[v].size(); i++) {
                        EdgeID e_bar = Q.new_edge(v,new_edges[v][i]);
                        Q.setEdgeWeight(e_bar, 1);
                }
        } endfor

        Q.finish_construction();
        if(Q.number_of_edges() != G.number_of_edges()){
                std::cout <<  "problem: number of edges in dyn setting do not match"  << std::endl;
                std::cout <<  "edge counter " <<  edge_counter  << std::endl;
                std::cout <<  "Q " <<  Q.number_of_edges() << std::endl;
                std::cout <<  "G " <<  G.number_of_edges()  << std::endl;
                exit(0);
        }
}
