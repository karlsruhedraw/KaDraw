//
// Author: Christian Schulz <christian.schulz@kit.edu>
// 

#ifndef QUALITY_METRICS_4AJHZVX0
#define QUALITY_METRICS_4AJHZVX0

#include "data_structure/graph_access.h"

class quality_metrics {
public:
        quality_metrics();
        virtual ~quality_metrics();

        void print_distances( graph_access & G);

        double full_stress_measure_unit_weight( graph_access & G );
        double maxent_unitweight( graph_access & G, double q, double alpha, std::string prefix );
        double maxent_unitweight( graph_access & G, double q, double alpha );
        double avg_infeasibility_per_edge( graph_access & G );
        double compute_fsm_scaling_factor_unit_weight( graph_access & G ); 
};


#endif /* end of include guard: QUALITY_METRICS_4AJHZVX0 */
