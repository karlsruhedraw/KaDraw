/******************************************************************************
 * graph_drawer.h 
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

#ifndef GRAPH_DRAWER_DHHOHNGO
#define GRAPH_DRAWER_DHHOHNGO

#include "coarsening/coarsening.h"
#include "uncoarsening/uncoarsening.h"
#include "data_structure/graph_access.h"
#include "config.h"
#include "tools/random_functions.h"
#include "tools/quality_metrics.h"
#include "tools/timer.h"
#include "uncoarsening/local_optimizer.h"

 
class graph_drawer {
public:
        graph_drawer();
        virtual ~graph_drawer();

        void perform_drawing( Config & config, graph_access & G) {
                coarsening coarsen;
                uncoarsening uncoarsen;
                graph_hierarchy hierarchy(config);

                timer t;

                t.restart();
                coarsen.perform_coarsening(config, G, hierarchy);
                std::cout <<  "coarsening took " <<  t.elapsed() << std::endl;

                graph_access& Q = *hierarchy.get_coarsest();
                if(Q.number_of_nodes() > 2 || Q.number_of_nodes() < 2) {
                        forall_nodes(Q, node) {
                                Q.setCoords(node, random_functions::nextDouble(0,1),random_functions::nextDouble(0,1));
                        } endfor
                } else {
                        //place them at perfect distance
                        Q.setCoords(0, 0,0);
                        double dist   = config.intercluster_distance_factor;
                        dist *= (sqrt(Q.getNodeWeight(0))+sqrt(Q.getNodeWeight(1)))/2.0;
                        dist /= config.general_distance_scaling_factor;

                        Q.setCoords(1, 0, dist);
                        std::cout <<  "current setting to distance " <<  dist  << std::endl;
                }
                
                uncoarsen.perform_uncoarsening(config, hierarchy);
        };
};


#endif /* end of include guard: GRAPH_DRAWER_DHHOHNGO */
