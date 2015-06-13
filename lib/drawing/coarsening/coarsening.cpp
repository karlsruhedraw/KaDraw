/******************************************************************************
 * coarsening.cpp 
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

#include <limits>
#include <sstream>

#include "coarsening.h"
#include "coarsening_configurator.h"
#include "contraction.h"
#include "data_structure/graph_hierarchy.h"
#include "definitions.h"
#include "graph_io.h"
#include "stop_rules/stop_rules.h"

coarsening::coarsening() {

}

coarsening::~coarsening() {

}

void coarsening::perform_coarsening(const Config & config, graph_access & G, graph_hierarchy & hierarchy) {

        NodeID no_of_coarser_vertices = G.number_of_nodes();
        NodeID no_of_finer_vertices   = G.number_of_nodes();

        CoarseMapping* coarse_mapping = NULL;

        graph_access* finer                      = &G;
        matching* edge_matcher                   = NULL;
        contraction* contracter                  = new contraction();
        Config copy_of_config = config;

        stop_rule* coarsening_stop_rule = new strong_stop_rule(copy_of_config, G.number_of_nodes());
 
        coarsening_configurator coarsening_config;

        unsigned int level    = 0;
        bool contraction_stop = false;
        do {
                graph_access* coarser = new graph_access();
                coarse_mapping        = new CoarseMapping();
                Matching edge_matching;
                NodePermutationMap permutation;

                coarsening_config.configure_coarsening(copy_of_config, &edge_matcher, level);

                copy_of_config.upper_bound_partition = std::min(pow(config.size_base,level+1), ceil(config.upper_bound_partition/copy_of_config.cluster_coarsening_factor));
                copy_of_config.upper_bound_partition = std::min(copy_of_config.upper_bound_partition, G.number_of_nodes()-1);

                edge_matcher->match(copy_of_config, *finer, edge_matching, 
                                    *coarse_mapping, no_of_coarser_vertices, permutation);

                if( level == 0 ) {
                        forall_nodes_parallel(G, node) {
                                G.setPartitionIndex(node, (*coarse_mapping)[node]);
                        } endfor
                }
                
                delete edge_matcher; 

                contracter->contract(copy_of_config, *finer, *coarser, edge_matching, 
                                     *coarse_mapping, no_of_coarser_vertices, permutation);

                hierarchy.push_back(finer, coarse_mapping);
                contraction_stop = coarsening_stop_rule->stop(no_of_finer_vertices, no_of_coarser_vertices) && coarser->number_of_edges() != 0;
              
                no_of_finer_vertices = no_of_coarser_vertices;
                PRINT(std::cout <<  "no of coarser vertices " << no_of_coarser_vertices   
                                <<  " and no of edges " <<  coarser->number_of_edges() << std::endl;)

                if( finer->number_of_nodes()/no_of_coarser_vertices < 1.1) {
                        copy_of_config.cluster_coarsening_factor *= 0.7;
                }
                if( level == 0 ) {
                        G.set_partition_count(no_of_coarser_vertices);
                }

                finer = coarser;

                level++;
        } while( contraction_stop ); 

        hierarchy.push_back(finer, NULL); // append the last created level

        delete contracter;
        delete coarsening_stop_rule;
}


