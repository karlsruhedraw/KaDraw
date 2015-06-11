/******************************************************************************
 * local_optimizer.h 
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

#ifndef LOCAL_OPTIMIZER_1NLF0Q4Y
#define LOCAL_OPTIMIZER_1NLF0Q4Y

#include <unordered_map>
#include "config.h"
#include "data_structure/graph_access.h"
#include "tools/random_functions.h"
#include "io/graph_io.h"

struct coord_t {
        CoordType x;
        CoordType y;
        coord_t() : x(0), y(0) {

        }

};


class local_optimizer {
        public:
                local_optimizer();
                virtual ~local_optimizer();

                void run_maxent_optimization( const Config & config, graph_access & G, graph_access * coarse_graph = NULL, CoarseMapping * coarse_mapping = NULL); 

        private:   
                void configure_distances( const Config & config, graph_access & G, std::vector< CoordType > & distances  );
                void run_maxent_optimization_internal( const Config & config, graph_access & G );
                void run_maxent_optimization_internal_fast_approx( const Config & config, graph_access & G, graph_access & Q, CoarseMapping* coarse_mapping );
                void scale_graph_coordinates( const Config & config, PartitionID block, graph_access & G, graph_access * Q ) ;
                void construct_two_hop( graph_access & G, graph_access & Q );
};


#endif /* end of include guard: LOCAL_OPTIMIZER_1NLF0Q4Y */
