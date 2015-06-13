/******************************************************************************
 * graph_hierarchy.cpp 
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


#include "graph_hierarchy.h"
#include "tools/random_functions.h"

graph_hierarchy::graph_hierarchy(Config draw_config) : m_current_coarser_graph(NULL), 
                                                       m_current_coarse_mapping(NULL),
                                                       m_current_level(-1) {
        config = draw_config;
}

graph_hierarchy::~graph_hierarchy() {
        for( unsigned i = 1; i < m_full_graph_hierarchy.size(); i++) {
                if(m_full_graph_hierarchy[i] != NULL)
                        delete m_full_graph_hierarchy[i];
        }

        for( unsigned i = 0; i < m_to_delete_mappings.size(); i++) {
                if(m_to_delete_mappings[i] != NULL)
                        delete m_to_delete_mappings[i];
        }


        for( unsigned i = 0; i+1 < m_to_delete_hierachies.size(); i++) {
                if(m_to_delete_hierachies[i] != NULL)
                delete m_to_delete_hierachies[i];
        }
}

void graph_hierarchy::push_back(graph_access * G, CoarseMapping * coarse_mapping) {
        m_the_graph_hierarchy.push(G);
        m_full_graph_hierarchy.push_back(G);
        m_the_mappings.push(coarse_mapping);
        m_full_mappings.push_back(coarse_mapping);
	m_to_delete_mappings.push_back(coarse_mapping);
        m_coarsest_graph = G;
        m_current_level++;
}

graph_access* graph_hierarchy::pop_finer_and_project() {
        graph_access* finer = pop_coarsest();

        CoarseMapping* coarse_mapping = m_the_mappings.top(); // mapps finer to coarser nodes
        m_the_mappings.pop(); 

        if(finer == m_coarsest_graph) {
                m_current_coarser_graph = finer;
                finer = pop_coarsest();
               
                coarse_mapping = m_the_mappings.top(); 
                m_the_mappings.pop();
        }
        
        ASSERT_EQ(m_the_graph_hierarchy.size(), m_the_mappings.size());

        //perform projection
        graph_access& fRef = *finer;
        graph_access& cRef = *m_current_coarser_graph;

        if(config.use_polar_coordinates) {
                forall_nodes(fRef, n) {
                        NodeID coarser_node = (*coarse_mapping)[n];
                        CoordType max_dist = config.intracluster_distance_factor;
                        max_dist *= sqrt(cRef.getNodeWeight(coarser_node))/2.0;
                        max_dist *= config.general_distance_scaling_factor;

                        // project coordinates
                        double angle = random_functions::nextDouble(0,2*3.1415);
                        double distance = random_functions::nextDouble(0, max_dist);
                        CoordType x = distance * cos( angle );
                        CoordType y = distance * sin( angle );
                        fRef.setCoords(n, cRef.getX(coarser_node) + x, cRef.getY(coarser_node) + y); 
                } endfor
        } else {
                forall_nodes(fRef, n) {
                        NodeID coarser_node = (*coarse_mapping)[n];
                        // project coordinates
                        fRef.setCoords(n, cRef.getX(coarser_node)+random_functions::nextDouble(0.01,0.05), cRef.getY(coarser_node) +random_functions::nextDouble(0.01,0.05)); 
                } endfor
        }

        m_current_coarse_mapping = coarse_mapping;
        m_current_coarser_graph = finer;
        m_current_level--;

        return finer;                
}

CoarseMapping * graph_hierarchy::get_mapping_of_current_finer() {
        return m_current_coarse_mapping; 
}

CoarseMapping * graph_hierarchy::get_mapping_plus_x(int num_levels) {
        CoarseMapping * new_mapping = new CoarseMapping;
        new_mapping->resize(m_full_graph_hierarchy[m_current_level]->number_of_nodes());
        graph_access * current_finer = m_full_graph_hierarchy[m_current_level];
        forall_nodes_parallel((*current_finer), node) {
                (*new_mapping)[node] = node;
        } endfor

        for( int i = m_current_level; i < m_current_level+num_levels && i < (int)m_full_graph_hierarchy.size()-1; i++) {
                CoarseMapping * current_cmap  = m_full_mappings[i];
                forall_nodes_parallel((*current_finer), node) {
                        (*new_mapping)[node] = (*current_cmap)[(*new_mapping)[node]];
                } endfor
        }

        return new_mapping;
}

CoarseMapping * graph_hierarchy::get_mapping_plus_x_faster(int num_levels) {
        int cur_pos = std::min(m_current_level+num_levels, (int)m_full_graph_hierarchy.size()-1)-1;

        // create initial tmp_mapping
        CoarseMapping * tmp_mapping  = new CoarseMapping;
        graph_access * current_coarser = m_full_graph_hierarchy[cur_pos+1];
        tmp_mapping->resize(current_coarser->number_of_nodes());
        forall_nodes_parallel((*current_coarser), node) {
                (*tmp_mapping)[node] = node;
        } endfor

        while( cur_pos >= m_current_level ) {
                graph_access * current_Q       = m_full_graph_hierarchy[cur_pos];
                CoarseMapping * current_cmap   = m_full_mappings[cur_pos];
                CoarseMapping * new_cmap       = new CoarseMapping;
                new_cmap->resize(current_Q->number_of_nodes());
        
                forall_nodes_parallel((*current_Q), node) {
                        (*new_cmap)[node] = (*tmp_mapping)[(*current_cmap)[node]];
                } endfor
                delete tmp_mapping;
                tmp_mapping = new_cmap;
                cur_pos--;
        }

        return tmp_mapping;
}

graph_access * graph_hierarchy::get_coarser_plus_x(int num_levels) {
        if(m_current_level + num_levels < (int)m_full_graph_hierarchy.size()) {
                return m_full_graph_hierarchy[m_current_level + num_levels];
        } else {
                return m_coarsest_graph;
        } 
}

graph_access* graph_hierarchy::get_coarsest( ) {
        return m_coarsest_graph;                
}

graph_access* graph_hierarchy::pop_coarsest( ) {
        graph_access* current_coarsest = m_the_graph_hierarchy.top(); 
        m_the_graph_hierarchy.pop();
        return current_coarsest;                
}

bool graph_hierarchy::isEmpty( ) {
        ASSERT_EQ(m_the_graph_hierarchy.size(), m_the_mappings.size());
        return m_the_graph_hierarchy.empty();        
}

unsigned int graph_hierarchy::size() {
        return m_the_graph_hierarchy.size();        
}
