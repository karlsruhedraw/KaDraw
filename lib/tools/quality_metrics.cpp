//
// Author: Christian Schulz <christian.schulz@kit.edu>
// 

#include <iomanip>
#include "algorithms/shortest_paths.h"
#include "quality_metrics.h"

quality_metrics::quality_metrics() {

}

quality_metrics::~quality_metrics() {

}

void quality_metrics::print_distances( graph_access & G) {
        forall_nodes(G, node) {
                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        if( node < target ) {
                                double distance = sqrt((G.getX(node) - G.getX(target))*(G.getX(node) - G.getX(target))+ (G.getY(node) - G.getY(target))*(G.getY(node) - G.getY(target)));
                                std::cout <<  "distance node " <<  node <<  " target " <<  target << " is " <<  std::setprecision(200) <<  distance << std::endl;
                        }
                } endfor          
        } endfor
}

double quality_metrics::maxent_unitweight( graph_access & G, double q, double alpha, std::string prefix ) {
        double energy  = 0;
        double entropy = 0;

        forall_nodes_parallel_reduce(G, source, +, entropy) {
                forall_nodes(G, target) {
                        if(source == target) continue;

                        double diffX       = G.getX(source) - G.getX(target);
                        double diffY       = G.getY(source) - G.getY(target);
                        double dist_square = diffX*diffX+diffY*diffY;
                        double dist        = sqrt(dist_square);

                        if(abs(q) < 0.001) {
                                entropy += log(dist);
                        } else {
                                entropy += pow(dist,-q);
                        }
                } endfor
        } endfor

        forall_nodes(G, source) {
                forall_out_edges(G, e, source) {
                        NodeID target = G.getEdgeTarget(e);
                        double diffX       = G.getX(source) - G.getX(target);
                        double diffY       = G.getY(source) - G.getY(target);
                        double dist_square = diffX*diffX+diffY*diffY;
                        double dist        = sqrt(dist_square);

                        int best_distance = 1;
                        energy += (dist - best_distance)*(dist - best_distance)/(best_distance*best_distance);

                        // we added those in the previous iteration but they do not belong here
                        if(abs(q) < 0.001) {
                                entropy -= log(dist);
                        } else {
                                entropy -= pow(dist,-q);
                        }

                } endfor
        } endfor

        if(abs(q) > 0.001) {
               entropy *= -sgn(q);
        }

        std::cout <<  prefix << "sgn(q="<<q<<") is " << sgn(q) << std::endl;
        std::cout <<  prefix << "energy is " <<  energy << std::endl;
        std::cout <<  prefix << "entropy is " <<  entropy  << std::endl;
        std::cout <<  prefix << "alpha*entropy is " <<  alpha*entropy  << std::endl;
        std::cout <<  prefix <<"alpha is " << std::setprecision(4) <<  alpha  << std::endl;

        energy -= alpha*entropy;
        return energy;
}


double quality_metrics::maxent_unitweight( graph_access & G, double q, double alpha ) {
        double energy  = 0;
        double entropy = 0;

        forall_nodes_parallel_reduce(G, source, +, entropy) {
                forall_nodes(G, target) {
                        if(source == target) continue;

                        double diffX       = G.getX(source) - G.getX(target);
                        double diffY       = G.getY(source) - G.getY(target);
                        double dist_square = diffX*diffX+diffY*diffY;
                        double dist        = sqrt(dist_square);

                        if(abs(q) < 0.001) {
                                entropy += log(dist);
                        } else {
                                entropy += pow(dist,-q);
                        }
                } endfor
        } endfor

        forall_nodes(G, source) {
                forall_out_edges(G, e, source) {
                        NodeID target = G.getEdgeTarget(e);
                        double diffX       = G.getX(source) - G.getX(target);
                        double diffY       = G.getY(source) - G.getY(target);
                        double dist_square = diffX*diffX+diffY*diffY;
                        double dist        = sqrt(dist_square);

                        int best_distance = 1;
                        energy += (dist - best_distance)*(dist - best_distance)/(best_distance*best_distance);

                        // we added those in the previous iteration but they do not belong here
                        if(abs(q) < 0.001) {
                                entropy -= log(dist);
                        } else {
                                entropy -= pow(dist,-q);
                        }

                } endfor
        } endfor

        if(abs(q) > 0.001) {
               entropy *= -sgn(q);
        }

        std::cout <<  "sgn(q="<<q<<") is " << sgn(q) << std::endl;
        std::cout <<  "energy is " <<  energy << std::endl;
        std::cout <<  "entropy is " <<  entropy  << std::endl;
        std::cout <<  "alpha*entropy is " <<  alpha*entropy  << std::endl;
        std::cout <<  "alpha is " << std::setprecision(4) <<  alpha  << std::endl;

        energy -= alpha*entropy;
        return energy/2;
}

double quality_metrics::full_stress_measure_unit_weight( graph_access & G ) {
        double energy = 0;
        double scaling_factor = compute_fsm_scaling_factor_unit_weight(G);
        std::cout <<  "scaling factor is " << scaling_factor  << std::endl;
        forall_nodes_parallel_reduce(G, source,+,energy) {
	        shortest_paths sp;
	        std::vector<int> deepth(G.number_of_nodes(), -1);
                sp.one_to_many_unit_weight(G, source, deepth);

                forall_nodes(G, target) {
                        double diffX       = G.getX(source) - G.getX(target);
                        double diffY       = G.getY(source) - G.getY(target);
                        double dist_square = diffX*diffX+diffY*diffY;
                        double dist        = sqrt(dist_square);

                        int best_distance = deepth[target];
                        if( best_distance == 0 ) continue;
                        energy += (scaling_factor*dist - best_distance)*(scaling_factor*dist - best_distance)/(best_distance*best_distance);
                } endfor
        } endfor

        return energy/2;
}

double quality_metrics::compute_fsm_scaling_factor_unit_weight( graph_access & G ) {
        double top_fraction    = 0;
        double bottom_fraction = 0;
        forall_nodes_parallel_reduce(G, source,+,top_fraction) {
                shortest_paths sp;
	        std::vector<int> deepth(G.number_of_nodes(), -1);
                sp.one_to_many_unit_weight(G, source, deepth);

                forall_nodes(G, target) {
                        double diffX       = G.getX(source) - G.getX(target);
                        double diffY       = G.getY(source) - G.getY(target);
                        double dist_square = diffX*diffX+diffY*diffY;
                        double dist        = sqrt(dist_square);

                        int best_distance = deepth[target];
                        if( best_distance == 0 ) continue;
                        top_fraction += (dist / best_distance);

                } endfor
        } endfor

        forall_nodes_parallel_reduce(G, source,+,bottom_fraction) {
                shortest_paths sp;
	        std::vector<int> deepth(G.number_of_nodes(), -1);
                sp.one_to_many_unit_weight(G, source, deepth);

                forall_nodes(G, target) {
                        double diffX       = G.getX(source) - G.getX(target);
                        double diffY       = G.getY(source) - G.getY(target);
                        double dist_square = diffX*diffX+diffY*diffY;
                        double dist        = sqrt(dist_square);

                        int best_distance = deepth[target];
                        if( best_distance == 0 ) continue;
                        bottom_fraction += (dist*dist) / (best_distance*best_distance);

                } endfor
        } endfor


        return top_fraction/bottom_fraction;
}

double quality_metrics::compute_sparse_scaling_factor_unit_weight( graph_access & G ) {
        double top_fraction    = 0;
        double bottom_fraction = 0;
        forall_nodes_parallel_reduce(G, source,+,top_fraction) {

                forall_out_edges(G, e, source) {
                        NodeID target = G.getEdgeTarget(e);
                        double diffX       = G.getX(source) - G.getX(target);
                        double diffY       = G.getY(source) - G.getY(target);
                        double dist_square = diffX*diffX+diffY*diffY;
                        double dist        = sqrt(dist_square);

                        int best_distance = 1;
                        if( best_distance == 0 ) continue;
                        top_fraction += (dist / best_distance);

                } endfor
        } endfor

        forall_nodes_parallel_reduce(G, source,+,bottom_fraction) {

                forall_out_edges(G, e, source) {
                        NodeID target = G.getEdgeTarget(e);
                        double diffX       = G.getX(source) - G.getX(target);
                        double diffY       = G.getY(source) - G.getY(target);
                        double dist_square = diffX*diffX+diffY*diffY;
                        double dist        = sqrt(dist_square);

                        int best_distance = 1;
                        if( best_distance == 0 ) continue;
                        bottom_fraction += (dist*dist) / (best_distance*best_distance);

                } endfor
        } endfor


        return top_fraction/bottom_fraction;
}


double quality_metrics::avg_infeasibility_per_edge( graph_access & G) {
        double total_infisibility = 0;

        forall_nodes(G, source) {
                forall_out_edges(G, e, source) {
                        NodeID target = G.getEdgeTarget(e);

                        double diffX       = G.getX(source) - G.getX(target);
                        double diffY       = G.getY(source) - G.getY(target);
                        double dist_square = diffX*diffX+diffY*diffY;
                        double dist        = sqrt(dist_square);

                        total_infisibility += fabs(1.0-dist);
                } endfor
        } endfor
        
        return total_infisibility/(double)G.number_of_edges();
}
