/******************************************************************************
 * kadraw.cpp 
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


#include <argtable2.h>
#include <iostream>
#include <math.h>
#include <regex.h>
#include <sstream>
#include <stdio.h>
#include <string.h> 
#include <iomanip>

#include "data_structure/graph_access.h"
#include "graph_io.h"
#include "tools/graph_extractor.h"
#include "tools/quality_metrics.h"
#include "macros_assertions.h"
#include "parse_parameters.h"
#include "drawing/graph_drawer.h"
#include "drawing/config.h"
#include "burn_drawing/burn_drawing.h"
#include "random_functions.h"
#include "timer.h"

int main(int argn, char **argv) {

        Config config;
        std::string graph_filename;

        bool is_graph_weighted = false;
        bool suppress_output   = false;
        bool recursive         = false;
       
        int ret_code = parse_parameters(argn, argv, 
                                        config, 
                                        graph_filename, 
                                        is_graph_weighted, 
                                        suppress_output, recursive); 

        if(ret_code) {
                return 0;
        }

        std::streambuf* backup = std::cout.rdbuf();
        std::ofstream ofs;
        ofs.open("/dev/null");
        if(suppress_output) {
                std::cout.rdbuf(ofs.rdbuf()); 
        }

        config.LogDump(stdout);
        graph_access G;     

        timer t;
        graph_io::readGraphWeighted(G, graph_filename);
        std::cout << "io time: " << t.elapsed()  << std::endl;
       

        if( G.number_of_nodes() < 100) {
                config.faster_drawing_num_levels = 3;
        } else if ( G.number_of_nodes() < 1000) {
                config.faster_drawing_num_levels = 5;
        } else if ( G.number_of_nodes() < 10000) {
                config.faster_drawing_num_levels = 6;
        } else if ( G.number_of_nodes() < 100000) {
                config.faster_drawing_num_levels = 7;
        } else if ( G.number_of_nodes() < 300000) {
                config.faster_drawing_num_levels = 8;
        } else if ( G.number_of_nodes() < 600000) {
                config.faster_drawing_num_levels = 9;
        } else if ( G.number_of_nodes() < 2000000) {
                config.faster_drawing_num_levels = 10;
        } else if ( G.number_of_nodes() < 4000000) {
                config.faster_drawing_num_levels = 11;
        } else if ( G.number_of_nodes() < 10000000) {
                config.faster_drawing_num_levels = 12;
        } else if ( G.number_of_nodes() < 20000000) {
                config.faster_drawing_num_levels = 15;
        } else if ( G.number_of_nodes() < 40000000) {
                config.faster_drawing_num_levels = 17;
        } else {
                config.faster_drawing_num_levels = 20;
        }

        srand(config.seed);
        random_functions::setSeed(config.seed);

        graph_access Q;
        graph_extractor E;
        E.extract_largest_component(G, Q);

        // **************************** compute coordinates *****************************************       
        graph_drawer gd;
        std::cout <<  "performing drawing!"  << std::endl;
        config.upper_bound_partition = Q.number_of_nodes()-1;
        gd.perform_drawing(config, Q);
        
        // ******************************* done ''drawing'' *****************************************       
        ofs.close();
        std::cout.rdbuf(backup);
        std::cout <<  "time spent " << t.elapsed()  << std::endl;

        
        quality_metrics qm;
        if(config.burn_image_to_disk) {
                std::cout <<  "now performing sparse scaling " <<  std::endl;
                double scaling_factor = qm.compute_sparse_scaling_factor_unit_weight(Q);
                std::cout <<  "sparse scaling factor is " <<  scaling_factor << std::endl;

                //scale coordinates
                forall_nodes(Q, node) {
                        Q.setCoords(node, Q.getX(node)*scaling_factor, Q.getY(node)*scaling_factor);
                } endfor

                std::cout <<  "now burning image to disk" << std::endl;
                t.restart();

                burn_drawing bd;
                bd.draw_graph(config, Q);

                std::cout <<  "took " << t.elapsed()  << std::endl;
        }

        if(config.compute_FSM || config.compute_MEnt || config.burn_coordinates_to_disk) {
                std::cout <<  "now strong computing scaling factor and scaling"  << std::endl;
                double scaling_factor = qm.compute_fsm_scaling_factor_unit_weight(Q);
                std::cout <<  "scaling factor is " <<  scaling_factor << std::endl;

                //scale coordinates
                forall_nodes(Q, node) {
                        Q.setCoords(node, Q.getX(node)*scaling_factor, Q.getY(node)*scaling_factor);
                } endfor
        }

        if(config.compute_FSM) {
                std::cout <<  "FSM " << std::setprecision(200) << qm.full_stress_measure_unit_weight(Q) << std::endl;
        }

        if(config.compute_MEnt) {
                std::cout <<  "MEnt " << std::setprecision(200) << qm.maxent_unitweight(Q, config.q, 0.008) << std::endl;
                std::cout <<  "Avg. Invisibility per Edge "  << std::setprecision(200) << qm.avg_infeasibility_per_edge(Q) << std::endl;
        }
        
        if(config.burn_coordinates_to_disk) {
                graph_io::writeCoordinates(Q, config.output_coord_filename);
        }
}
