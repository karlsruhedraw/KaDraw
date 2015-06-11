/******************************************************************************
 * complete_boundary.h 
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

#ifndef COMPLETE_BOUNDARY_URZZFDEI
#define COMPLETE_BOUNDARY_URZZFDEI

#include <execinfo.h>
#include <utility>

#include "boundary_lookup.h"
#include "data_structure/graph_access.h"
#include "partial_boundary.h"
#include "config.h"

struct block_informations {
        NodeWeight block_weight;
        NodeID block_no_nodes;
};

typedef std::vector<boundary_pair> QuotientGraphEdges;

class complete_boundary {
        public:
                complete_boundary(graph_access * G );
                virtual ~complete_boundary();

                void build();
                void build_from_coarser(complete_boundary * coarser_boundary, NodeID coarser_no_nodes, CoarseMapping * cmapping);

                inline void insert(NodeID node, PartitionID insert_node_into, boundary_pair * pair);
                inline void getUnderlyingQuotientGraph( graph_access & qgraph );

        private:
                //updates lazy values that the access functions need
                inline void update_lazy_values(boundary_pair * pair);
                
                //lazy members to avoid hashtable loop ups
                PartialBoundary*   m_pb_lhs_lazy;
                PartialBoundary*   m_pb_rhs_lazy;
                PartitionID        m_lazy_lhs;
                PartitionID        m_lazy_rhs;
                boundary_pair*     m_last_pair;
                size_t             m_last_key; 
                hash_boundary_pair m_hbp;

                graph_access * m_graph_ref;
                //implicit quotient graph structure
                block_pairs m_pairs;
                std::vector<block_informations> m_block_infos;
};



inline void complete_boundary::build() {
        graph_access & G = *m_graph_ref;

        for(PartitionID block = 0; block < G.get_partition_count(); block++) {
                m_block_infos[block].block_weight   = 0;
                m_block_infos[block].block_no_nodes = 0;
        }

        forall_nodes(G, n) {
                PartitionID source_partition = G.getPartitionIndex(n);
                m_block_infos[source_partition].block_weight   += G.getNodeWeight(n);
                m_block_infos[source_partition].block_no_nodes += 1;

                forall_out_edges(G, e, n) {
                        NodeID targetID              = G.getEdgeTarget(e);
                        PartitionID target_partition = G.getPartitionIndex(targetID);
                        bool is_cut_edge             = (source_partition != target_partition);

                        if(is_cut_edge) {
                                boundary_pair bp;
                                bp.k   = m_graph_ref->get_partition_count();
                                bp.lhs = source_partition;
                                bp.rhs = target_partition;
                                update_lazy_values(&bp);
                                m_pairs[bp].edge_cut += G.getEdgeWeight(e);    
                                insert(n, source_partition, &bp);
                        }
                } endfor
        } endfor

        block_pairs::iterator iter; 
        for(iter = m_pairs.begin(); iter != m_pairs.end(); iter++ ) { 
                data_boundary_pair& value = iter->second;
                value.edge_cut /= 2;
        }

}

inline void complete_boundary::insert(NodeID node, PartitionID insert_node_into, boundary_pair * pair) {
        update_lazy_values(pair);
        ASSERT_TRUE((m_lazy_lhs == pair->lhs && m_lazy_rhs == pair->rhs) 
                 || (m_lazy_lhs == pair->rhs && m_lazy_rhs == pair->lhs));

        if(insert_node_into == m_lazy_lhs) {
                ASSERT_EQ(m_graph_ref->getPartitionIndex(node),m_lazy_lhs);
                m_pb_lhs_lazy->insert(node);
        } else {
                ASSERT_EQ(m_graph_ref->getPartitionIndex(node),m_lazy_rhs);
                m_pb_rhs_lazy->insert(node);
        }    
}

inline void complete_boundary::update_lazy_values(boundary_pair * pair) {
        ASSERT_NEQ(pair->lhs, pair->rhs);
        
        boundary_pair & bp = *pair;
        size_t key = m_hbp(bp); 
        if(key != m_last_key) {
                data_boundary_pair & dbp = m_pairs[*pair]; 
                if(!dbp.initialized) {
                        m_pairs[*pair].lhs = pair->lhs;
                        m_pairs[*pair].rhs = pair->rhs;
                        dbp.initialized = true;
                }

                m_pb_lhs_lazy = &dbp.pb_lhs;
                m_pb_rhs_lazy = &dbp.pb_rhs;
                m_lazy_lhs    = dbp.lhs;
                m_lazy_rhs    = dbp.rhs;
                m_last_pair   = pair;
                m_last_key    = key;
        }
}

inline void complete_boundary::getUnderlyingQuotientGraph( graph_access & Q_bar ) {
         basicGraph * graphref = new basicGraph; 
         
         if(Q_bar.graphref != NULL) {
                delete Q_bar.graphref;
         }
         Q_bar.graphref = graphref;
 
         std::vector< std::vector< std::pair<PartitionID, EdgeWeight> > >  building_tool;
         building_tool.resize(m_block_infos.size());

         block_pairs::iterator iter; 
         for(iter = m_pairs.begin(); iter != m_pairs.end(); iter++ ) { 
                 boundary_pair cur_pair = iter->first;

                 std::pair<PartitionID, EdgeWeight> qedge_lhs;
                 qedge_lhs.first  = cur_pair.rhs;
                 qedge_lhs.second = m_pairs[cur_pair].edge_cut;
                 building_tool[cur_pair.lhs].push_back(qedge_lhs);

                 std::pair<PartitionID, EdgeWeight> qedge_rhs;
                 qedge_rhs.first  = cur_pair.lhs;
                 qedge_rhs.second = m_pairs[cur_pair].edge_cut;
                 building_tool[cur_pair.rhs].push_back(qedge_rhs);
         }

         Q_bar.start_construction(building_tool.size(), 2*m_pairs.size());
         
         for( unsigned p = 0; p < building_tool.size(); p++) {
                 NodeID node = Q_bar.new_node();
                 Q_bar.setNodeWeight(node,  m_block_infos[p].block_weight);

                 for( unsigned j = 0; j < building_tool[p].size(); j++) {
                         EdgeID e = Q_bar.new_edge(node, building_tool[p][j].first);
                         Q_bar.setEdgeWeight(e, building_tool[p][j].second);
                 }
         }

         Q_bar.finish_construction();
}

#endif /* end of include guard: COMPLETE_BOUNDARY_URZZFDEI */
