/******************************************************************************
 * uncoarsening.cpp 
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


#include "uncoarsening.h"
#include "local_optimizer.h"


uncoarsening::uncoarsening() {

}

uncoarsening::~uncoarsening() {

}

int uncoarsening::perform_uncoarsening(const Config & config, graph_hierarchy & hierarchy) {
        Config cfg = config;
        local_optimizer lopt;
        graph_access * coarsest = hierarchy.get_coarsest();
        PRINT(std::cout << "log>" << "unrolling graph with " << coarsest->number_of_nodes() << std::endl;)

        Config tmp = config;
        tmp.draw_cluster_first = false;
        lopt.run_maxent_optimization(tmp, *coarsest, NULL, NULL);

        graph_access* coarser = NULL;

        while(!hierarchy.isEmpty()) {
                graph_access* G = hierarchy.pop_finer_and_project();
                PRINT(std::cout << "log>" << "unrolling graph with " << G->number_of_nodes()<<  std::endl;)

                CoarseMapping* coarse_mapping = hierarchy.get_mapping_of_current_finer();
                
                //call refinement
                if( G->number_of_edges() ) {
                        Config cfg = config;
                        cfg.last_level = hierarchy.isEmpty();
                        if( config.faster_drawing && config.faster_drawing_num_levels > 1 ) {
                                CoarseMapping* direct_coarse_mapping = NULL;
                                if(config.faster_mapping) {
                                        direct_coarse_mapping = hierarchy.get_mapping_plus_x_faster(config.faster_drawing_num_levels);
                                } else {
                                        direct_coarse_mapping = hierarchy.get_mapping_plus_x(config.faster_drawing_num_levels);
                                }

                                graph_access*  direct_coarser = hierarchy.get_coarser_plus_x(config.faster_drawing_num_levels);
                                lopt.run_maxent_optimization(cfg, *G, direct_coarser, direct_coarse_mapping);
                                delete direct_coarse_mapping;
                        } else {
                                lopt.run_maxent_optimization(cfg, *G, coarser, coarse_mapping);
                        }
                }

		if(!hierarchy.isEmpty()) {
			coarser = G;
		}

        }

        return 0;
}
