/******************************************************************************
 * evaluator.cpp 
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


#include <argtable3.h>
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

        timer t;
        graph_access G;  
        graph_io::readGraphWeighted(G, graph_filename);
        graph_io::readCoordinates(G, config.coord_filename);
                
        std::cout << "io time: " << t.elapsed()  << std::endl;
       
        quality_metrics qm;
        if(config.print_final_distances) {
                qm.print_distances(G);
        }

        double scaling_factor = qm.compute_fsm_scaling_factor_unit_weight(G);
        std::cout <<  "scaling factor is " <<  scaling_factor << std::endl;
        forall_nodes(G, node) {
                G.setCoords(node, G.getX(node)*scaling_factor, G.getY(node)*scaling_factor);
        } endfor

        forall_nodes(G, node) {
                double rnd = random_functions::nextDouble(1e-7,1e-4);
                double rnd2 = random_functions::nextDouble(1e-7,1e-4);
                std::cout <<  "rnd " <<  rnd  << std::endl;
                G.setCoords(node, G.getX(node)  + rnd , G.getY(node) + rnd2);
        } endfor

        
        if(config.compute_FSM) {
                std::cout <<  "FSM " << std::setprecision(200) << qm.full_stress_measure_unit_weight(G) << std::endl;
        }

        if(config.compute_MEnt) {
                std::cout <<  "MEnt " << std::setprecision(200) << qm.maxent_unitweight(G, config.q, 0.008) << std::endl;
                std::cout <<  "Avg. Invisibility per Edge "  << std::setprecision(200) << qm.avg_infeasibility_per_edge(G) << std::endl;
        }
}
