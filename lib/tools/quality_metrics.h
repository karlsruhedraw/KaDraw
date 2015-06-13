/******************************************************************************
 * quality_metrics.h 
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


#ifndef QUALITY_METRICS_4AJHZVX0
#define QUALITY_METRICS_4AJHZVX0

#include "data_structure/graph_access.h"

class quality_metrics {
public:
        quality_metrics();
        virtual ~quality_metrics();

        void print_distances( graph_access & G);

        double full_stress_measure_unit_weight( graph_access & G );
        double maxent_unitweight( graph_access & G, double q, double alpha, std::string prefix );
        double maxent_unitweight( graph_access & G, double q, double alpha );
        double avg_infeasibility_per_edge( graph_access & G );
        double compute_fsm_scaling_factor_unit_weight( graph_access & G ); 
        double compute_sparse_scaling_factor_unit_weight( graph_access & G ); 
};


#endif /* end of include guard: QUALITY_METRICS_4AJHZVX0 */
