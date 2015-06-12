/******************************************************************************
 * definitions.h 
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

#ifndef DEFINITIONS_H_CHR
#define DEFINITIONS_H_CHR

#include <limits>
#include <queue>
#include <vector>

#include "limits.h"
#include "macros_assertions.h"
#include "stdio.h"

// allows us to disable most of the output during partitioning
        #define PRINT(x) x

/**********************************************
 * Constants
 * ********************************************/
//Types needed for the graph ds
typedef unsigned int 	NodeID;
typedef float           EdgeRatingType;
typedef unsigned int 	EdgeID;
typedef unsigned int 	PathID;
typedef unsigned int 	PartitionID;
typedef unsigned int 	NodeWeight;
typedef int 		EdgeWeight;
typedef EdgeWeight 	Gain;
typedef int 		Color;
typedef unsigned int 	Count;
typedef float           CoordType;
typedef std::vector<NodeID> boundary_starting_nodes;

const EdgeID UNDEFINED_EDGE            = std::numeric_limits<EdgeID>::max();
const NodeID NOTMAPPED                 = std::numeric_limits<EdgeID>::max();
const NodeID UNDEFINED_NODE            = std::numeric_limits<NodeID>::max();
const PartitionID INVALID_PARTITION    = std::numeric_limits<PartitionID>::max();
const PartitionID BOUNDARY_STRIPE_NODE = std::numeric_limits<PartitionID>::max();
const int NOTINQUEUE 		       = std::numeric_limits<int>::max();
const int ROOT 			       = 0;

//for the gpa algorithm
struct edge_source_pair {
        EdgeID e;
        NodeID source;       
};

struct source_target_pair {
        NodeID source;       
        NodeID target;       
};

//matching array has size (no_of_nodes), so for entry in this table we get the matched neighbor
typedef std::vector<NodeID> CoarseMapping;
typedef std::vector<NodeID> Matching;
typedef std::vector<NodeID> NodePermutationMap;

typedef enum {
        PERMUTATION_QUALITY_NONE, 
	PERMUTATION_QUALITY_FAST,  
	PERMUTATION_QUALITY_GOOD
} PermutationQuality;

typedef enum {
        CLUSTER_COARSENING
} MatchingType;

typedef enum {
        STOP_RULE_SIMPLE, 
	STOP_RULE_MULTIPLE_K, 
	STOP_RULE_STRONG 
} StopRule;

typedef enum {
        RANDOM_NODEORDERING, 
        DEGREE_NODEORDERING
} NodeOrderingType;

// An enum to identify the supported export graphics types.
typedef enum {
  GRAPHICS_TYPE_INVALID,
  GRAPHICS_TYPE_PNG,
  GRAPHICS_TYPE_PDF
} GraphicsFormatType;


#endif

