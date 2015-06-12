/******************************************************************************
 * coarsening_configurator.h 
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

#ifndef COARSENING_CONFIGURATOR_8UJ78WYS
#define COARSENING_CONFIGURATOR_8UJ78WYS

#include "contraction.h"
#include "data_structure/graph_hierarchy.h"
#include "definitions.h"
#include "clustering/size_constraint_label_propagation.h"
#include "stop_rules/stop_rules.h"

class coarsening_configurator {
        public:
                coarsening_configurator( ) {};
                virtual ~coarsening_configurator() {};

                void configure_coarsening(const Config & config, 
                                          matching** edge_matcher, 
                                          unsigned level); 
};

inline void coarsening_configurator::configure_coarsening( const Config & config, 
                                                           matching** edge_matcher, 
                                                           unsigned level) {

        switch(config.matching_type) {
                case CLUSTER_COARSENING:
                        PRINT(std::cout <<  "cluster_coarsening"  << std::endl;)
                        *edge_matcher = new size_constraint_label_propagation();
                        break;

        }
}

#endif /* end of include guard: COARSENING_CONFIGURATOR_8UJ78WYS */
