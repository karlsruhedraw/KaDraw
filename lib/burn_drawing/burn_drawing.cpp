/******************************************************************************
 * burn_drawing.cpp 
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

// Draw the graph using the Cairo Library

#include <cairo.h>
#include <cairo-pdf.h>
#include <algorithm>
#include "burn_drawing.h"


burn_drawing::burn_drawing() {

}

burn_drawing::~burn_drawing() {

}

void burn_drawing::draw_graph( Config & config, graph_access & G) {
        GraphicsFormatType export_type = config.export_grafic_type;

        
        double scale            = config.image_scale;
        const double border     = 0.0;
        const int max_dim_px    = 1200;
        const double line_width = config.linewidth;

        cairo_t *cr;
        cairo_surface_t *surface;

        double x_min = G.getX(0);
        double x_max = G.getX(0);
        double y_min = G.getY(0);
        double y_max = G.getY(0);

        forall_nodes(G, node) {
                x_min = std::min(x_min, G.getX(node));
                x_max = std::max(x_max, G.getX(node));
                y_min = std::min(y_min, G.getY(node));
                y_max = std::max(y_max, G.getY(node));
        } endfor

        float width  = x_max - x_min + 2 * border;
        float height = y_max - y_min + 2 * border;

        // Get width and height in pixels.
        int width_px = 0;
        int height_px = 0;
        if (export_type == GRAPHICS_TYPE_PNG) {
                if (width > height) {
                        width_px = max_dim_px;
                        height_px = round(1.0 * height / width * max_dim_px);
                        scale = 1.0 * max_dim_px / width;
                } else {
                        height_px = max_dim_px;
                        width_px = round(1.0 * width / height * max_dim_px);
                        scale = 1.0 * max_dim_px / height;
                }
        }
        printf("x max = %f, x min = %f, x delta = %f, y max = %f, y min = %f, y delta = %f\n",
                        x_max, x_min, x_max - x_min, y_max, y_min, y_max - y_min);
        printf("width = %f, height = %f\n", width, height);
        printf("width_px = %d, height_px = %d\n", width_px, height_px);
        printf("scale = %f\n", scale);

        //// Initialize Cairo surface.
        if (export_type == GRAPHICS_TYPE_PDF) {
                surface = cairo_pdf_surface_create(config.output_filename.c_str(), width * scale + 2 * border, height * scale + 2 * border);
        } else { // PNG
                std::cout <<  "creating a png"  << std::endl;
                surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width_px, height_px);
        }
        cr = cairo_create(surface);
        // White background.
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_rectangle(cr, 0, 0,  width * scale + 2 * border, height * scale + 2 * border);
        cairo_fill(cr);
        cairo_translate(cr, border - x_min * scale, border - y_min * scale);
        cairo_scale(cr, scale, scale);
        // Initialize graphic style.
        cairo_set_line_width(cr, line_width / scale);
        // Generate colors.
        double r = 0.0, g = 0.0, b = 0.0;
        cairo_set_source_rgb(cr, r, g, b);

        //// Now, render the graph.
        if( config.draw_initial_clustering ) {
                int num_colors = G.get_partition_count();
                num_colors = std::max(1, num_colors); // if no clustering should be plotted
                std::vector<int> hues(num_colors);
		std::cout <<  "num colors used " <<  num_colors << std::endl;
                for (int i = 1, iend = num_colors; i != iend; ++i) {
                        hues[i] = 360.0 * i / num_colors;
                }

                forall_nodes(G, node) {
                        double x = G.getX(node);
                        double y = G.getY(node);

                        forall_out_edges(G, e, node) {
                                NodeID target = G.getEdgeTarget(e);

                                if(G.getPartitionIndex(node) == G.getPartitionIndex(target)) {
                                        HsvToRgb(hues[G.getPartitionIndex(node)], 0.8, 1.0, &r, &g, &b);
                                        cairo_set_source_rgb(cr, r, g, b);
                                } else {
                                        if( config.light_intercluster_edges ) {
                                                cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
                                        } else {
                                                cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
                                        }
                                }

                                double x_target = G.getX(target);
                                double y_target = G.getY(target);
                                cairo_move_to(cr, x, y);
                                cairo_line_to(cr, x_target, y_target);
                                cairo_stroke(cr);

                        } endfor
                } endfor
        } else {
                // else color edges based on their length
                int num_colors = 3;
                std::vector<int> hues(num_colors);
                for (int i = 1, iend = num_colors; i != iend; ++i) {
                        hues[i] = 360.0 * (i-1) / num_colors;
                }

                std::vector< double > edge_lengths(G.number_of_edges());
                forall_nodes(G, node) {
                        forall_out_edges(G, e, node) {
                                NodeID target = G.getEdgeTarget(e);
                                edge_lengths[e] = sqrt((G.getX(node) - G.getX(target))*(G.getX(node) - G.getX(target)) + (G.getY(node) - G.getY(target))*(G.getY(node) - G.getY(target)));
                        } endfor
                } endfor

                std::sort(edge_lengths.begin(), edge_lengths.end()); 

                double median = 0;
                if( G.number_of_edges() > 0 ) {
                        if( edge_lengths.size() % 2 == 0) {
                                median += edge_lengths[edge_lengths.size()/2-1];
                                median += edge_lengths[edge_lengths.size()/2];
                                median /= 2;
                        } else {
                                median = edge_lengths[edge_lengths.size()/2];
                        }
                } else {
                        std::cout <<  "attention: graph has no edges"  << std::endl;
                }

                forall_nodes(G, node) {
                        double x = G.getX(node);
                        double y = G.getY(node);

                        forall_out_edges(G, e, node) {
                                NodeID target = G.getEdgeTarget(e);

                                double distance   = (G.getX(node) - G.getX(target))*(G.getX(node) - G.getX(target));
                                distance         += (G.getY(node) - G.getY(target))*(G.getY(node) - G.getY(target));
                                distance          = sqrt(distance);

                                if(distance < 0.5*median) {
                                        HsvToRgb(hues[1], 0.8, 1.0, &r, &g, &b);
                                } else if (distance > 1.5*median) {
                                        HsvToRgb(hues[1], 0.8, .75, &r, &g, &b);
                                } else {
                                        //interpolate
                                        double alpha = 0;
                                        if( distance > median ) {
                                                alpha = (1.5*median - distance)/median;
                                        } else {
                                                alpha = (distance-0.5*median)/median;
                                        }
                                        HsvToRgb(hues[2], 0.9, .5, &r, &g, &b);
                                }
                                cairo_set_source_rgb(cr, r, g, b);

                                double x_target = G.getX(target);
                                double y_target = G.getY(target);
                                cairo_move_to(cr, x, y);
                                cairo_line_to(cr, x_target, y_target);
                                cairo_stroke(cr);

                        } endfor
                } endfor       
        }

        if (export_type == GRAPHICS_TYPE_PNG) {
                std::cout <<  "writing PNG to " << config.output_filename.c_str()  << std::endl;
                cairo_surface_write_to_png(surface, config.output_filename.c_str());
        }

        cairo_destroy(cr);
        cairo_surface_destroy(surface);


}
