/******************************************************************************
 * shortest_paths.h 
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

#ifndef SHORTEST_PATHS_HQGDJB3G
#define SHORTEST_PATHS_HQGDJB3G

#include <vector>
#include "data_structure/graph_access.h"

class shortest_paths {
public:
        shortest_paths();
        virtual ~shortest_paths();

        void one_to_many_unit_weight( graph_access & G, NodeID & source, std::vector< int > & distances);
};


#endif /* end of include guard: SHORTEST_PATHS_HQGDJB3G */
