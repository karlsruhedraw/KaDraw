/******************************************************************************
 * config.h 
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

#ifndef CONFIG_DI1ES4T0
#define CONFIG_DI1ES4T0

#include "definitions.h"

// Configuration for the drawing.
struct Config
{
        Config() {}


        //============================================================
        //=======================MATCHING=============================
        //============================================================
        PermutationQuality permutation_quality;

        MatchingType matching_type;
        
        NodeWeight max_vertex_weight; 
        
        NodeWeight largest_graph_weight; 
        
        bool disable_max_vertex_weight_constraint;

        //=======================================
        //===============MISC====================
        //=======================================

        int seed;

        bool fast;

        bool strong;

        std::string graph_filename;

        std::string output_filename;

        NodeWeight upper_bound_partition; 

        StopRule stop_rule;

        int num_vert_stop_factor;

        bool draw_initial_clustering;

        //=======================================
        //===========LABEL PROPAGATION===========
        //=======================================
        NodeOrderingType node_ordering;

        double cluster_coarsening_factor; 

        int label_iterations;

        int number_of_clusterings;

        double balance_factor;

        int repetitions;

        NodeWeight cluster_upperbound;
        //=======================================
        //======== MAXENT LOCAL OPT =============
        //=======================================
        
        double maxent_alpha;

        double maxent_min_alpha;

        double q;

        int maxent_inner_iterations;

        int maxent_outer_iterations;

        double maxent_tol;

        double size_base;

        double intercluster_distance_factor;

        double intracluster_distance_factor;

        double general_distance_scaling_factor;

        double general_distance_scaling_factor_clusterings;

        bool draw_cluster_first;

        bool draw_cluster_first_disable_fine_tune;

        double image_scale;

        bool last_level; 

        bool compute_FSM;

        bool compute_MEnt;

        bool light_intercluster_edges;

        bool print_final_distances;

        bool burn_image_to_disk;

        bool burn_coordinates_to_disk;

        bool use_polar_coordinates;

        std::string coord_filename;

        std::string output_coord_filename;

        bool faster_drawing;

        int faster_drawing_num_levels;

        bool faster_mapping;

        bool disable_scaling;

        //=======================================
        //============BURN DRAWING===============
        //=======================================
        
        GraphicsFormatType export_grafic_type;

        double linewidth;


        void LogDump(FILE *out) const {
        }
};


#endif /* end of include guard: CONFIG_DI1ES4T0 */
