/******************************************************************************
 * stop_rules.h 
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

#ifndef STOP_RULES_SZ45JQS6
#define STOP_RULES_SZ45JQS6

#include <math.h>

#include "config.h"

class stop_rule {
        public:
                stop_rule() {};
                virtual ~stop_rule() {};
                virtual bool stop( NodeID number_of_finer_vertices, NodeID number_of_coarser_vertices ) = 0;
};

class strong_stop_rule : public stop_rule {
        public:
                strong_stop_rule(Config & config, NodeID number_of_nodes) {
                        num_stop = 2;
                };
                virtual ~strong_stop_rule() {};
                bool stop( NodeID number_of_finer_vertices, NodeID number_of_coarser_vertices );

        private:
                NodeID num_stop;
};

inline bool strong_stop_rule::stop(NodeID no_of_finer_vertices, NodeID no_of_coarser_vertices ) {
        return no_of_coarser_vertices > num_stop;
}

#endif /* end of include guard: STOP_RULES_SZ45JQS6 */
