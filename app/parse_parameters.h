/******************************************************************************
 * parse_parameters.h 
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


#ifndef PARSE_PARAMETERS_GPJMGSM8
#define PARSE_PARAMETERS_GPJMGSM8

#include <omp.h>
#include "configuration.h"

int parse_parameters(int argn, char **argv, 
                     Config & config, 
                     std::string & graph_filename, 
                     bool & is_graph_weighted, 
                     bool & suppress_program_output, 
                     bool & recursive) {

        const char *progname = argv[0];

        // Setup argtable parameters.
        struct arg_lit *help                                 = arg_lit0(NULL, "help","Print help.");
        struct arg_str *filename                             = arg_strn(NULL, NULL, "FILE", 1, 1, "Path to graph file to draw.");
        struct arg_str *output_filename                      = arg_str0(NULL, "output_filename", NULL, "Output filename of the png/pdf file.");
        struct arg_str *output_coord_filename                = arg_str0(NULL, "output_coord_filename", NULL, "Output filename of the coordinates.");
        struct arg_str *coord_filename                       = arg_str0(NULL, "coordfilename", NULL, "Filename of input coordinates to evaluate.");
        struct arg_int *user_seed                            = arg_int0(NULL, "seed", NULL, "Seed to use for the PRNG.");
        struct arg_int *maxent_inner_iter                    = arg_int0(NULL, "maxent_inner_iter", NULL, "Number of inner max ent iterations.");
        struct arg_int *maxent_outer_iter                    = arg_int0(NULL, "maxent_outer_iter", NULL, "Number of outer max ent iterations.");
        struct arg_dbl *maxent_tol                           = arg_dbl0(NULL, "maxent_tol", NULL, "Tol to break max ent iterations.");
        struct arg_dbl *maxent_alpha                         = arg_dbl0(NULL, "maxent_alpha", NULL, "Initial alpha value of the max ent algorithm.");
        struct arg_dbl *maxent_min_alpha                     = arg_dbl0(NULL, "maxent_min_alpha", NULL, "Minimum alpha value of the max ent algorithm.");
        struct arg_dbl *lp_factor                            = arg_dbl0(NULL, "lp_factor", NULL, "Factor for label propagation.");
        struct arg_dbl *size_base                            = arg_dbl0(NULL, "size_base", NULL, "Size base for the cluster size constraint of label propagation.");
        struct arg_lit *draw_initial_clustering              = arg_lit0(NULL, "draw_initial_clustering","Draw initial clustering.");
        struct arg_lit *burn_image_to_disk                   = arg_lit0(NULL, "burn_image_to_disk","Save the image in a file.");
        struct arg_lit *burn_coordinates_to_disk             = arg_lit0(NULL, "burn_coordinates_to_disk","Save the coordinates in a file.");
        struct arg_lit *use_polar_coordinates                = arg_lit0(NULL, "use_polar_coordinates","Uses polar coordinates during projection of nodes.");
        struct arg_lit *faster_drawing                       = arg_lit0(NULL, "faster_drawing","Uses faster drawing by approximating repulsive forces.");
        struct arg_lit *disable_faster_mapping               = arg_lit0(NULL, "disable_faster_mapping","Disables the usage of faster algorithm to compute mapping for more general h-level approach.");
        struct arg_int *faster_drawing_num_levels            = arg_int0(NULL, "faster_drawing_num_levels", NULL, "Set the number of levels to skip in faster drawing algorithm.");
        struct arg_dbl *intercluster_distance_factor         = arg_dbl0(NULL, "intercluster_distance_factor", NULL, "Factor for intercluster distance (default 1)");
        struct arg_dbl *intracluster_distance_factor         = arg_dbl0(NULL, "intracluster_distance_factor", NULL, "Factor for intracluster distance (default 1)");
        struct arg_dbl *general_distance_scaling_factor      = arg_dbl0(NULL, "general_distance_scaling_factor", NULL, "General factor for distance scaling.");

        //struct arg_lit *disable_scaling                      = arg_lit0(NULL, "disable_scaling","Disable scaling.");
        struct arg_lit *draw_cluster_first                   = arg_lit0(NULL, "draw_cluster_first","Draw each cluster first while keeping the others unpacked.");
        struct arg_int *num_threads                          = arg_int0(NULL, "num_threads", NULL, "Set the number of OMP threads.");
        struct arg_lit *draw_cluster_first_disable_fine_tune = arg_lit0(NULL, "draw_cluster_first_disable_fine_tune","Disable the n^2 step when drawing clustering first.");
        struct arg_lit *compute_FSM                          = arg_lit0(NULL, "compute_FSM","Enable computation of Full Stress Measure.");
        struct arg_lit *compute_MEnt                         = arg_lit0(NULL, "compute_MEnt","Enable computation of MaxEnt-stress.");
        struct arg_lit *print_final_distances                = arg_lit0(NULL, "print_final_distances","Print the final distances.");
        struct arg_lit *light_intercluster_edges             = arg_lit0(NULL, "light_intercluster_edges","Enable draw intercluster edges in light gray.");
        struct arg_dbl *image_scale                          = arg_dbl0(NULL, "image_scale", NULL, "Set image scale.");
        struct arg_dbl *linewidth                            = arg_dbl0(NULL, "linewidth", NULL, "Line width to use for drawing.");
        struct arg_dbl *q                                    = arg_dbl0(NULL, "q", NULL, "Parameter q in the MaxEnt formular.");
        struct arg_int *label_propagation_iterations         = arg_int0(NULL, "label_propagation_iterations", NULL, "Number of LP iterations.");
        struct arg_rex *export_type                          = arg_rex0(NULL, "export_type","^(pdf|png)$","TYPE", REG_EXTENDED, "Specify export type. [pdf|png]");
        struct arg_rex *preconfiguration                     = arg_rex0(NULL, "preconfiguration", "^(strong|eco|fast)$", "VARIANT", REG_EXTENDED, "Use a preconfiguration. (Default: fast) [strong|eco|fast]." );


        struct arg_end *end                                  = arg_end(100);

        // Define argtable.
        void* argtable[] = {
                help, filename, 
#ifndef MODE_GRAPHTOMTX
#ifndef MODE_EVALUATOR 
#ifndef MODE_DRAWFROMCOORDS                
                user_seed,
                preconfiguration, 
                //label_propagation_iterations,
                //maxent_inner_iter,
                //maxent_outer_iter,
                //maxent_alpha,
                //maxent_tol,
                //maxent_min_alpha,
                //q,
                //size_base,
                //lp_factor,
                //draw_initial_clustering, 
                //intercluster_distance_factor,
                //intracluster_distance_factor,
                //general_distance_scaling_factor,
                //faster_drawing,
                //disable_faster_mapping,
                //faster_drawing_num_levels,
                burn_coordinates_to_disk,
                //use_polar_coordinates,
                burn_image_to_disk,
                output_filename, 
                image_scale,
                num_threads,
                export_type,
                linewidth,
                //print_final_distances,
                compute_FSM,
                compute_MEnt,
                //disable_scaling,
#endif
#endif
#ifdef MODE_EVALUATOR
                print_final_distances,
                compute_FSM,
                compute_MEnt,
                coord_filename,
#endif
#ifdef MODE_DRAWFROMCOORDS
                output_filename, 
                coord_filename,
                export_type,
#endif
#endif

                end
        };
        // Parse arguments.
        int nerrors = arg_parse(argn, argv, argtable);

        // Catch case that help was requested.
        if (help->count > 0) {
                printf("Usage: %s", progname);
                arg_print_syntax(stdout, argtable, "\n");
                arg_print_glossary(stdout, argtable,"  %-40s %s\n");
                arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
                return 1;
        }


        if (nerrors > 0) {
                arg_print_errors(stderr, end, progname);
                printf("Try '%s --help' for more information.\n",progname);
                arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
                return 1; 
        }

        if(filename->count > 0) {
                graph_filename = filename->sval[0];
        }

        if(output_filename->count > 0) {
                config.output_filename = output_filename->sval[0];
        }

        if(output_coord_filename->count > 0) {
                config.output_coord_filename = output_coord_filename->sval[0];
        }

        recursive = false;

        configuration cfg;
        cfg.standard(config);
        cfg.fast(config);

        if(preconfiguration->count > 0) {

                if (strcmp("fast", preconfiguration->sval[0]) == 0) {
                        cfg.fast(config);
                } else if (strcmp("eco", preconfiguration->sval[0]) == 0) {
                        cfg.eco(config);
                } else if (strcmp("strong", preconfiguration->sval[0]) == 0) {
                        cfg.strong(config);
                } else {
                        fprintf(stderr, "Invalid preconfiguration variant: \"%s\"\n", preconfiguration->sval[0]);
                        exit(0);
                }
        }

        if(export_type->count > 0) {
                if (strcmp("png", export_type->sval[0]) == 0) {
                        config.export_grafic_type = GRAPHICS_TYPE_PNG;
                        if(!config.output_filename.compare("image.pdf")) {
                                config.output_filename = std::string("image.png");
                        }
                } else if (strcmp("pdf", export_type->sval[0]) == 0) {
                        config.export_grafic_type = GRAPHICS_TYPE_PDF;
                } else {
                        fprintf(stderr, "Invalid export type variant: \"%s\"\n", export_type->sval[0]);
                        exit(0);
                }
        }

        if(coord_filename->count > 0) {
                config.coord_filename = coord_filename->sval[0];
        } 

        if(print_final_distances->count > 0)  {
                config.print_final_distances = true;
        }

        if(faster_drawing->count > 0)  {
                config.faster_drawing = true;
        }

        if(disable_faster_mapping->count > 0)  {
                config.faster_mapping = false;
        }

        if(faster_drawing_num_levels->count > 0)  {
                config.faster_drawing_num_levels = faster_drawing_num_levels->ival[0];
                if(config.faster_drawing_num_levels == 0) {
                        config.faster_mapping = false;
                }
        }

        if(compute_FSM->count > 0)  {
                config.compute_FSM = true;
        }

        if(compute_MEnt->count > 0)  {
                config.compute_MEnt = true;
        }

        if(light_intercluster_edges->count > 0)  {
                config.light_intercluster_edges = true;
        }

        if(burn_image_to_disk->count > 0)  {
                config.burn_image_to_disk = true;
        }

        if(burn_coordinates_to_disk->count > 0)  {
                config.burn_coordinates_to_disk = true;
        }

        if(use_polar_coordinates) {
                config.use_polar_coordinates = true;
        }

        if(draw_cluster_first->count > 0)  {
                config.draw_cluster_first = true;
        }

        if(draw_cluster_first_disable_fine_tune->count > 0)  {
                config.draw_cluster_first_disable_fine_tune = true;
        }

        if(draw_initial_clustering->count > 0)  {
                config.draw_initial_clustering = true;
        }

        if(image_scale->count > 0)  {
                config.image_scale = image_scale->dval[0];
        }

        if(label_propagation_iterations->count > 0)  {
                config.label_iterations = label_propagation_iterations->ival[0];
        }

        if(maxent_inner_iter->count > 0)  {
                config.maxent_inner_iterations = maxent_inner_iter->ival[0];
        }

        if(maxent_outer_iter->count > 0)  {
                config.maxent_outer_iterations = maxent_outer_iter->ival[0];
        }

        if(maxent_tol ->count > 0)  {
                config.maxent_tol = maxent_tol->dval[0];
        }

        if(maxent_alpha ->count > 0)  {
                config.maxent_alpha = maxent_alpha->dval[0];
        }

        if(maxent_min_alpha->count > 0)  {
                config.maxent_min_alpha = maxent_min_alpha->dval[0];
        }

        if(q->count > 0)  {
                config.q = q->dval[0];
        }

        if(linewidth->count > 0)  {
                config.linewidth = linewidth->dval[0];
        }

        if(lp_factor->count > 0)  {
                config.cluster_coarsening_factor = lp_factor->dval[0];
        }

        if(size_base->count > 0)  {
                config.size_base = size_base->dval[0];
        }

        if(intercluster_distance_factor->count > 0)  {
                config.intercluster_distance_factor = intercluster_distance_factor->dval[0];
        }

        if(intracluster_distance_factor->count > 0)  {
                config.intracluster_distance_factor = intracluster_distance_factor->dval[0];
        }

        if(general_distance_scaling_factor->count > 0)  {
                config.general_distance_scaling_factor = general_distance_scaling_factor->dval[0];
        }

        if(num_threads->count > 0)  {
                omp_set_num_threads(num_threads->ival[0]);
        }
        return 0;

}

#endif /* end of include guard: PARSE_PARAMETERS_GPJMGSM8 */
