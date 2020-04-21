//
// Created by kdy on 2020/4/17.
//

#ifndef GRAPHLAB_GRAPH_ALGO_ARRAY_CONVERT_H
#define GRAPHLAB_GRAPH_ALGO_ARRAY_CONVERT_H

#include "../../../Graph_Algo/core/AbstractGraph.h"

namespace graphlab {

    template<typename vertex_data, typename edge_data>
    void graphlab_to_algo_vertex_set(std::vector<Vertex> &vSet, int local_vertex_num,
                                     graphlab::distributed_graph<vertex_data, edge_data> &graph) {

        for (int i = 0; i < local_vertex_num; i++) {
            vSet.at(i).outDegree = graph.l_vertex(i).num_out_edges();
            vSet.at(i).inDegree = graph.l_vertex(i).num_in_edges();
            vSet.at(i).isMaster = graph.l_is_master(i);
        }
    }
}

#endif //GRAPHLAB_GRAPH_ALGO_ARRAY_CONVERT_H
