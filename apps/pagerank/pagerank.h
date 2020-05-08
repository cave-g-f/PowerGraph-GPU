//
// Created by kdy on 2020/5/4.
//

#ifndef GRAPHLAB_PAGERANK_H
#define GRAPHLAB_PAGERANK_H

#include <vector>
#include <string>
#include <fstream>

#include <graphlab.hpp>

struct vertex_data : graphlab::IS_POD_TYPE {
    double delta;
    double rank;

    vertex_data(double _delta = 0, double _rank = 0) :
            delta(_delta), rank(_rank) {}
};

typedef graphlab::distributed_graph<vertex_data, double> pagerank_graph_type;

typedef double pagerank_message;

class pagerank
        : public graphlab::ivertex_program<pagerank_graph_type, graphlab::empty, pagerank_message>,
          public graphlab::IS_POD_TYPE {

    double message_value;
    bool scatter_active = false;
    double reset_prob = 0.15;
    double delta_threshold = 0.001;

public:
    void init(icontext_type &context, const vertex_type &vertex, const pagerank_message &msg) {
        message_value = msg;
    }

    edge_dir_type gather_edges(icontext_type &context, const vertex_type &vertex) const {
        return graphlab::NO_EDGES;
    };

    void apply(icontext_type &context, vertex_type &vertex, const graphlab::empty &empty) {
        scatter_active = false;
        vertex.data().delta = message_value;
        vertex.data().rank += message_value;
        if (vertex.data().delta > delta_threshold) scatter_active = true;
    }

    edge_dir_type scatter_edges(icontext_type &context, const vertex_type &vertex) const {
        if (scatter_active)
            return graphlab::OUT_EDGES;
        else return graphlab::NO_EDGES;
    }; // end of scatter_edges

    void scatter(icontext_type &context, const vertex_type &vertex, edge_type &edge) const {
        pagerank_message msg;
        msg = vertex.data().delta * (1 - reset_prob) / edge.source().num_out_edges();
        context.signal(edge.target(), msg);
    } // end of scatter

};

#endif //GRAPHLAB_PAGERANK_H
