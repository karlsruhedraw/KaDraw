/******************************************************************************
 * configuration.h 
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


#ifndef CONFIGURATION_3APG5V7Z
#define CONFIGURATION_3APG5V7Z

#include "drawing/config.h"

class configuration {
        public:
                configuration() {} ;
                virtual ~configuration() {};

                void standard( Config & config );
                void standardsnw( Config & config );

                void fast( Config & config );
                void eco( Config & config );
                void strong( Config & config ); 
};


inline void configuration::standard( Config & config ) {
        config.seed                                        = 0;
        config.permutation_quality                         = PERMUTATION_QUALITY_FAST;

        //  networking parameters
        config.cluster_coarsening_factor                   = 20;
        config.label_iterations                            = 5;
        config.number_of_clusterings                       = 1;
        config.repetitions                                 = 1;
        config.node_ordering                               = DEGREE_NODEORDERING;

        config.maxent_alpha                                = 1;
        config.maxent_min_alpha                            = 0.008;
        config.maxent_inner_iterations                     = 50;
        config.maxent_outer_iterations                     = 10;
        config.maxent_tol                                  = 0.0001;
        config.size_base                                   = 2;
        config.draw_initial_clustering                     = false;
        config.linewidth                                   = 0.5;
        config.export_grafic_type                          = GRAPHICS_TYPE_PDF;
        config.intercluster_distance_factor                = 1;
        config.intracluster_distance_factor                = 1;
        config.general_distance_scaling_factor             = 1;
        config.general_distance_scaling_factor_clusterings = 0.5;
        config.draw_cluster_first                          = false;
        config.draw_cluster_first_disable_fine_tune        = false;
        config.image_scale                                 = 10.0;
        config.compute_FSM                                 = false;
        config.compute_MEnt                                = false;
        config.light_intercluster_edges                    = false;
        config.print_final_distances 		           = false;
        config.burn_image_to_disk                          = false;
        config.burn_coordinates_to_disk                    = false;
        config.q                                           = 0;
        config.use_polar_coordinates                       = true;
        config.faster_drawing                              = false;
        config.faster_mapping                              = true;
        config.faster_drawing_num_levels                   = 1;
        config.disable_scaling                             = false;

        if(!config.output_filename.compare("")) {
                config.output_filename = std::string("image.pdf");
        }

        if(!config.output_coord_filename.compare("")) {
                config.output_coord_filename = std::string("image.coord");
        }

        config.coord_filename = "";

}

inline void configuration::standardsnw( Config & config ) {
        config.matching_type        = CLUSTER_COARSENING;
        config.stop_rule            = STOP_RULE_MULTIPLE_K;
        config.num_vert_stop_factor = 5000;
}

inline void configuration::fast( Config & config ) {
        standard(config);
        standardsnw(config);
        config.maxent_inner_iterations                     = 2;
        config.maxent_outer_iterations                     = 13;
        config.maxent_tol                                  = 0.0001;
}

inline void configuration::eco( Config & config ) {
        standard(config);
        standardsnw(config);

        config.maxent_inner_iterations                     = 2;
        config.maxent_outer_iterations                     = 12;
        config.maxent_tol                                  = 0.00001;

}

inline void configuration::strong( Config & config ) {
        standard(config);
        standardsnw(config);

        config.maxent_inner_iterations                     = 3;
        config.maxent_outer_iterations                     = 20;
        config.maxent_tol                                  = 0.000001;
}

#endif /* end of include guard: CONFIGURATION_3APG5V7Z */
