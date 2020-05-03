//
// Created by kdy on 2020/4/20.
//

#include <graphlab/graph_algo/graph_algo_connect_gas.cpp>

#include "../../../apps/sssp/sssp.h"
#include "../../../apps/label_propagation/label_propagation.h"

template class graphlab::algo_to_gas<double, double, double, min_distance_type>;
template class graphlab::algo_to_gas<LPA_Value, LPA_MSG, int, label_counter>;

