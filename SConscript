#/******************************************************************************
# * SConscript
# *
# * Source of KaDraw -- Karlsruhe Graph Drawing.
# *
# ******************************************************************************
# * Copyright (C) 2013 Christian Schulz <christian.schulz@kit.edu>
# *
# * This program is free software: you can redistribute it and/or modify it
# * under the terms of the GNU General Public License as published by the Free
# * Software Foundation, either version 3 of the License, or (at your option)
# * any later version.
# *
# * This program is distributed in the hope that it will be useful, but WITHOUT
# * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# * more details.
# *
# * You should have received a copy of the GNU General Public License along with
# * this program.  If not, see <http://www.gnu.org/licenses/>.
# *****************************************************************************/


# The main SConscript file for the code.
#
# We simply import the main environment and then define the targets.  This
# submodule contains a sequential matching and contraction code and tests for
# the code.
import platform
import sys

# Get the current platform.
SYSTEM = platform.uname()[0]

Import('env')

# Build a library from the code in lib/.
libdrawit_files = [   'lib/data_structure/graph_hierarchy.cpp',
                      'lib/algorithms/shortest_paths.cpp',
                      'lib/io/graph_io.cpp',
                      'lib/tools/random_functions.cpp',
                      'lib/tools/graph_extractor.cpp',
                      'lib/tools/quality_metrics.cpp',
                      'lib/drawing/coarsening/coarsening.cpp',
                      'lib/drawing/coarsening/contraction.cpp',
                      'lib/drawing/coarsening/matching/matching.cpp',
                      'lib/drawing/coarsening/clustering/node_ordering.cpp',
                      'lib/drawing/coarsening/clustering/size_constraint_label_propagation.cpp',
                      'lib/drawing/uncoarsening/uncoarsening.cpp',
                      'lib/drawing/graph_drawer.cpp',
                      'lib/drawing/uncoarsening/complete_boundary.cpp', 
                      'lib/drawing/uncoarsening/local_optimizer.cpp', 
                      'lib/drawing/uncoarsening/partial_boundary.cpp',
                      'lib/burn_drawing/burn_drawing.cpp' 
                  ]


if env['program'] == 'kadraw':
        env.Program('kadraw', ['app/kadraw.cpp']+libdrawit_files, LIBS=['libargtable2','gomp','cairo'])

if env['program'] == 'evaluator':
        env.Append(CXXFLAGS = '-DMODE_EVALUATOR')
        env.Append(CCFLAGS  = '-DMODE_EVALUATOR')
        env.Program('evaluator', ['app/evaluator.cpp']+libdrawit_files, LIBS=['libargtable2','gomp','cairo'])

if env['program'] == 'draw_from_coordinates':
        env.Append(CXXFLAGS = '-DMODE_DRAWFROMCOORDS')
        env.Append(CCFLAGS  = '-DMODE_DRAWFROMCOORDS')
        env.Program('draw_from_coordinates', ['app/draw_from_coordinates.cpp']+libdrawit_files, LIBS=['libargtable2','gomp','cairo'])

if env['program'] == 'graphchecker':
        env.Append(CXXFLAGS = '-DMODE_GRAPHCHECKER')
        env.Append(CCFLAGS  = '-DMODE_GRAPHCHECKER')
        env.Program('graphchecker', ['app/graphchecker.cpp'], LIBS=['libargtable2','gomp'])

