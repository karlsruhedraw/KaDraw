/******************************************************************************
 * uncoarsening.cpp 
 *
 * Source of DrawIT 
 *
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
        if(!config.last_level_only) 
                lopt.run_maxent_optimization(tmp, *coarsest, NULL, NULL);

        graph_access* coarser = NULL;
        graph_access* finest = NULL;

        while(!hierarchy.isEmpty()) {
                graph_access* G = hierarchy.pop_finer_and_project();
                finest = G;
                PRINT(std::cout << "log>" << "unrolling graph with " << G->number_of_nodes()<<  std::endl;)

                CoarseMapping* coarse_mapping = hierarchy.get_mapping_of_current_finer();
                
                //call refinement
                if( !config.last_level_only && G->number_of_edges() ) {
                        Config cfg = config;
                        cfg.last_level = hierarchy.isEmpty();
                        if( config.faster_drawing && config.faster_drawing_num_levels > 1 ) {
                                CoarseMapping* direct_coarse_mapping = NULL;
                                if(config.faster_mapping) {
                                        std::cout <<  "faster mapping computation"  << std::endl;
                                        direct_coarse_mapping = hierarchy.get_mapping_plus_x_faster(config.faster_drawing_num_levels);
                                } else {
                                        direct_coarse_mapping = hierarchy.get_mapping_plus_x(config.faster_drawing_num_levels);
                                }

                                graph_access*  direct_coarser = hierarchy.get_coarser_plus_x(config.faster_drawing_num_levels);
                                std::cout <<  "mapping size " <<  direct_coarser->number_of_nodes()  << std::endl;
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

        if(config.last_level_only) {
                std::cout <<  "improving coordinates last level only"  << std::endl;
                //uncoarsening performed, now improve the given coordinates
                if( config.faster_drawing && config.faster_drawing_num_levels > 1 ) {
                        CoarseMapping* direct_coarse_mapping = NULL;
                        if(config.faster_mapping) {
                                std::cout <<  "faster mapping computation"  << std::endl;
                                direct_coarse_mapping = hierarchy.get_mapping_plus_x_faster(config.faster_drawing_num_levels);
                        } else {
                                direct_coarse_mapping = hierarchy.get_mapping_plus_x(config.faster_drawing_num_levels);
                        }

                        graph_access*  direct_coarser = hierarchy.get_coarser_plus_x(config.faster_drawing_num_levels);
                        std::cout <<  "mapping size " <<  direct_coarser->number_of_nodes()  << std::endl;
                        lopt.run_maxent_optimization(cfg, *finest, direct_coarser, direct_coarse_mapping);
                        delete direct_coarse_mapping;
                } else {
                        CoarseMapping* coarse_mapping = hierarchy.get_mapping_of_current_finer();
                        lopt.run_maxent_optimization(cfg, *finest, coarser, coarse_mapping);
                }
        }

        return 0;
}
