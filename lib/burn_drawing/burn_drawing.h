/******************************************************************************
 * burn_drawing.h 
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


#ifndef BURN_DRAWING_UORVQGB6
#define BURN_DRAWING_UORVQGB6

#include "config.h"
#include "data_structure/graph_access.h"



class burn_drawing {
public:
        burn_drawing();
        virtual ~burn_drawing();

        void draw_graph( Config & config, graph_access & G);

	void HsvToRgb(int h, double s, double v, double *r, double *g, double *b) {
		double H, S, V, R, G, B;
		double p1, p2, p3;
		double f;
		int i;

		if (s < 0) s = 0;
		if (v < 0) v = 0;
		if (s > 1) s = 1;
		if (v > 1) v = 1;

		S = s; V = v;
		H = (h % 360) / 60.0;
		i = H;
		f = H - i;
		p1 = V * (1 - S);
		p2 = V * (1 - (S * f));
		p3 = V * (1 - (S * (1 - f)));
		if      (i == 0) { R = V;  G = p3; B = p1; }
		else if (i == 1) { R = p2; G = V;  B = p1; }
		else if (i == 2) { R = p1; G = V;  B = p3; }
		else if (i == 3) { R = p1; G = p2; B = V;  }
		else if (i == 4) { R = p3; G = p1; B = V;  }
		else             { R = V;  G = p1; B = p2; }
		*r = R;
		*g = G;
		*b = B;
	}
};


#endif /* end of include guard: BURN_DRAWING_UORVQGB6 */
