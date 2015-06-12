/******************************************************************************
 * local_optimizer.cpp 
 *
 * Source of the DrawIt Program 
 ******************************************************************************
 * Copyright (C) 2014 Christian Schulz <christian.schulz@kit.edu>
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


#include <sstream>
#include <omp.h>
#include "local_optimizer.h"
#include "drawing/coarsening/edge_rating/edge_ratings.h"
#include "tools/graph_extractor.h"
#include "tools/quality_metrics.h"
#include "burn_drawing/burn_drawing.h"

local_optimizer::local_optimizer() {

}

local_optimizer::~local_optimizer() {

}

void local_optimizer::run_maxent_optimization( const Config & config, 
                graph_access & G, 
                graph_access * coarser_graph, 
                CoarseMapping* coarse_mapping ) {
        Config cfg = config;

        if( !(config.draw_cluster_first_disable_fine_tune && config.last_level) ) {
                if(config.faster_drawing) {
                        // use version that approximates repulsive forces 
                        if( coarser_graph == NULL) {
                                run_maxent_optimization_internal( config, G);
                        } else {
                                run_maxent_optimization_internal_fast_approx( config, G, *coarser_graph, coarse_mapping);
                        }
                } else {
                        run_maxent_optimization_internal( config, G);
                }
        }
}

void local_optimizer::run_maxent_optimization_internal( const Config & config, graph_access & G ) {
        if(G.number_of_edges() == 0) return;

        std::vector< coord_t > new_coord(G.number_of_nodes());
        CoordType alpha = config.maxent_alpha;
        int iterations  = config.maxent_inner_iterations;
        CoordType q     = config.q;

        std::vector<CoordType> distances(G.number_of_edges(),0); 
        configure_distances( config, G, distances);

        for( int i = 0; i < config.maxent_outer_iterations; i++) {

                CoordType norm_coords = 0;
                CoordType norm_diff = 0;
                do {
                        forall_nodes_parallel(G, node) {
                                CoordType rho_i = 0; // assume graph is connected?
                                if(G.getNodeDegree(node) == 0) continue;
                                forall_out_edges(G, e, node) {
                                        CoordType distance = distances[e];
                                        rho_i += 1/(distance*distance);
                                } endfor
                                rho_i = 1/rho_i;

                                CoordType S_x = 0;
                                CoordType S_y = 0;

                                CoordType n_S_x = 0;
                                CoordType n_S_y = 0;
                                forall_out_edges(G, e, node) {
                                        NodeID target      = G.getEdgeTarget(e);
                                        CoordType diffX       = G.getX(node) - G.getX(target);
                                        CoordType diffY       = G.getY(node) - G.getY(target);
                                        CoordType dist_square = diffX*diffX+diffY*diffY;
                                        CoordType distance    = distances[e];
                                        CoordType dist        = sqrt(dist_square);

                                        CoordType scaled_distance = distance/dist;
                                        CoordType squared_distance = distance*distance;

                                        S_x += (G.getX(target) + scaled_distance*diffX)/(squared_distance);
                                        S_y += (G.getY(target) + scaled_distance*diffY)/(squared_distance);

                                        CoordType dist_q = pow(dist, q+2);
                                        n_S_x -= diffX/dist_q;
                                        n_S_y -= diffY/dist_q;
                                } endfor

                                S_x *= rho_i;
                                S_y *= rho_i;

                                forall_nodes(G, target) {
                                        if( node == target ) continue;

                                        CoordType diffX       = G.getX(node) - G.getX(target);
                                        CoordType diffY       = G.getY(node) - G.getY(target);
                                        CoordType dist_square = diffX*diffX+diffY*diffY;
                                        CoordType dist        = sqrt(dist_square);

                                        CoordType dist_q = pow(dist, q+2);
                                        n_S_x += diffX/dist_q;
                                        n_S_y += diffY/dist_q;
                                } endfor
                                CoordType mult_factor = alpha*rho_i;

                                n_S_x *= mult_factor;
                                n_S_y *= mult_factor;

                                new_coord[node].x = S_x + sgn(q)*n_S_x;
                                new_coord[node].y = S_y + sgn(q)*n_S_y;
                        } endfor

                        norm_coords = 0;
                        norm_diff   = 0;

                        forall_nodes_parallel_reduce(G, node,+,norm_coords) {
                                norm_coords += G.getX(node)*G.getX(node);
                                norm_coords += G.getY(node)*G.getY(node);
                        } endfor
                        forall_nodes_parallel_reduce(G, node,+,norm_diff) {
                                norm_diff += (G.getX(node)-new_coord[node].x)* (G.getX(node)-new_coord[node].x);
                                norm_diff += (G.getY(node)-new_coord[node].y)* (G.getY(node)-new_coord[node].y);
                        } endfor

                        //update coordinates to new coordinate
                        forall_nodes_parallel(G, node) {
                                G.setCoords(node, new_coord[node].x, new_coord[node].y);
                        } endfor

                        if(norm_diff/norm_coords < config.maxent_tol) break;
                } while ( iterations-- > 0);

                if(norm_diff/norm_coords < config.maxent_tol) break;
                iterations = config.maxent_inner_iterations;
                alpha = std::max(0.3*alpha, config.maxent_min_alpha);
        }
};

void local_optimizer::run_maxent_optimization_internal_fast_approx( const Config & config, graph_access & G, graph_access & Q, CoarseMapping* coarse_mapping) {
        if(G.number_of_edges() == 0) return;

        std::vector< coord_t > new_coord(G.number_of_nodes());
        CoordType alpha = config.maxent_alpha;
        int iterations  = config.maxent_inner_iterations;
        CoordType q     = config.q;

        std::vector<CoordType> distances(G.number_of_edges(),0); 
        configure_distances(config, G, distances);

        // build cluster ID to nodes array
        NodeID num_clusters = Q.number_of_nodes();
        std::vector< std::vector< std::vector< NodeID > > > cluster_to_nodes_local(omp_get_max_threads());
        std::vector< NodeID > cluster_vertex_count(num_clusters,0);

        for( int i = 0; i < omp_get_max_threads(); i++) {
                cluster_to_nodes_local[i].resize(num_clusters);
        }
        forall_nodes_parallel(G, node) {
                NodeID cluster_id = (*coarse_mapping)[node];
                cluster_to_nodes_local[omp_get_thread_num()][cluster_id].push_back(node);
        } endfor

        //compute num nodes in cluster
        forall_nodes_parallel(Q, cluster_id) {
                NodeID cluster_size = 0;
                for( int thread_num = 0; thread_num < omp_get_max_threads(); thread_num++) {
                        cluster_size += cluster_to_nodes_local[thread_num][cluster_id].size();
                }
                cluster_vertex_count[cluster_id] = cluster_size;
        } endfor

        for( int i = 0; i < config.maxent_outer_iterations; i++) {
                CoordType norm_coords = 0;
                CoordType norm_diff = 0;
                do {

                        //update coordinates of coarse nodes
                        forall_nodes_parallel(Q, coarse_node) {
                                CoordType X_bar = 0;
                                CoordType Y_bar = 0;

                                for( int thread_num = 0; thread_num < omp_get_max_threads(); thread_num++) {
                                        for( unsigned int i = 0; i < cluster_to_nodes_local[thread_num][coarse_node].size(); i++) {
                                                NodeID cur_node = cluster_to_nodes_local[thread_num][coarse_node][i];
                                                X_bar += G.getNodeWeight(cur_node)*G.getX(cur_node);
                                                Y_bar += G.getNodeWeight(cur_node)*G.getY(cur_node);
                                        }
                                }
                                X_bar /= Q.getNodeWeight(coarse_node);
                                Y_bar /= Q.getNodeWeight(coarse_node);
                                Q.setCoords(coarse_node, X_bar, Y_bar);
                        } endfor

                        forall_nodes_parallel(G, node) {
                                CoordType rho_i = 0; // assume graph is connected?
                                if(G.getNodeDegree(node) == 0) continue;
                                forall_out_edges(G, e, node) {
                                        CoordType distance = distances[e];
                                        rho_i += 1/(distance*distance);
                                } endfor
                                rho_i = 1/rho_i;

                                CoordType S_x = 0;
                                CoordType S_y = 0;

                                CoordType n_S_x = 0;
                                CoordType n_S_y = 0;


                                forall_out_edges(G, e, node) {
                                        NodeID target      = G.getEdgeTarget(e);
                                        CoordType diffX       = G.getX(node) - G.getX(target);
                                        CoordType diffY       = G.getY(node) - G.getY(target);
                                        CoordType dist_square = diffX*diffX+diffY*diffY;
                                        CoordType distance    = distances[e];
                                        CoordType dist        = sqrt(dist_square);

                                        CoordType scaled_distance = distance/dist;
                                        CoordType squared_distance = distance*distance;

                                        S_x += (G.getX(target) + scaled_distance*diffX)/(squared_distance);
                                        S_y += (G.getY(target) + scaled_distance*diffY)/(squared_distance);

                                        CoordType dist_q = pow(dist, q+2);
                                        n_S_x -= diffX/dist_q;
                                        n_S_y -= diffY/dist_q;
                                } endfor

                                S_x *= rho_i;
                                S_y *= rho_i;

                                NodeID coarser_node = (*coarse_mapping)[node];
                                // compute repulsive forces on coarser level
                                forall_nodes(Q, target) {
                                        if( coarser_node == target ) continue;

                                        CoordType diffX       = G.getX(node) - Q.getX(target);
                                        CoordType diffY       = G.getY(node) - Q.getY(target);
                                        CoordType dist_square = diffX*diffX+diffY*diffY;
                                        CoordType dist        = sqrt(dist_square);

                                        CoordType dist_q = pow(dist, q+2);

                                        n_S_x += cluster_vertex_count[target]*diffX/dist_q;
                                        n_S_y += cluster_vertex_count[target]*diffY/dist_q;
                                } endfor

                                // compute repulsive within cluster explicitly
                                for( int thread_num = 0; thread_num < omp_get_max_threads(); thread_num++) {
                                        for( unsigned int i = 0; i < cluster_to_nodes_local[thread_num][coarser_node].size(); i++) {
                                                NodeID target = cluster_to_nodes_local[thread_num][coarser_node][i];
                                                if( node == target ) continue;

                                                CoordType diffX       = G.getX(node) - G.getX(target);
                                                CoordType diffY       = G.getY(node) - G.getY(target);
                                                CoordType dist_square = diffX*diffX+diffY*diffY;
                                                CoordType dist        = sqrt(dist_square);

                                                CoordType dist_q = pow(dist, q+2);
                                                n_S_x += diffX/dist_q;
                                                n_S_y += diffY/dist_q;
                                        }
                                }

                                n_S_x *= alpha*rho_i;
                                n_S_y *= alpha*rho_i;

                                new_coord[node].x = S_x + sgn(q)*n_S_x;
                                new_coord[node].y = S_y + sgn(q)*n_S_y;
                        } endfor

                        norm_coords = 0;
                        norm_diff   = 0;

                        forall_nodes_parallel_reduce(G, node,+,norm_coords) {
                                norm_coords += G.getX(node)*G.getX(node);
                                norm_coords += G.getY(node)*G.getY(node);
                        } endfor
                        forall_nodes_parallel_reduce(G, node,+,norm_diff) {
                                norm_diff += (G.getX(node)-new_coord[node].x)* (G.getX(node)-new_coord[node].x);
                                norm_diff += (G.getY(node)-new_coord[node].y)* (G.getY(node)-new_coord[node].y);
                        } endfor

                        //update coordinates to new coordinate
                        forall_nodes_parallel(G, node) {
                                G.setCoords(node, new_coord[node].x, new_coord[node].y);
                        } endfor


                        if(norm_diff/norm_coords < config.maxent_tol) {
                                break;
                        }
                } while ( iterations-- > 0);

                if(norm_diff/norm_coords < config.maxent_tol) break;
                iterations = config.maxent_inner_iterations;
                alpha = std::max(0.3*alpha, config.maxent_min_alpha);
        }
}

void local_optimizer::configure_distances( const Config & config, graph_access & G, std::vector< CoordType > & distances) {
        forall_nodes_parallel(G, node) {
                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);

                        CoordType factor =  config.intercluster_distance_factor;
                        if( G.getPartitionIndex(node) == G.getPartitionIndex(target)) 
                                factor =  config.intracluster_distance_factor;

                        distances[e]  = factor;
                        distances[e] *= (sqrt(G.getNodeWeight(node))+sqrt(G.getNodeWeight(target)))/2.0;
                        distances[e] /= config.general_distance_scaling_factor;
                } endfor
        } endfor
}


