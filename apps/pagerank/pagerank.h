//
// Created by kdy on 2020/5/4.
//

#ifndef GRAPHLAB_PAGERANK_H
#define GRAPHLAB_PAGERANK_H

#include <vector>
#include <string>
#include <fstream>

#include <graphlab.hpp>

#ifdef GRAPH_ALGO

#include "../../Graph_Algo/algo/PageRank/PageRank.h"

#endif

#define RESET_PROB 0.15
#define DELTA_TOL 0.001

struct pagerank_vertex_data : graphlab::IS_POD_TYPE {
    double delta;
    double rank;

    pagerank_vertex_data(double _delta = 0, double _rank = 0) : delta(_delta), rank(_rank) {}
};

typedef graphlab::distributed_graph<pagerank_vertex_data, double> pagerank_graph_type;

typedef double pagerank_message;

class pagerank
        : public graphlab::ivertex_program<pagerank_graph_type, graphlab::empty, pagerank_message>,
#ifdef GRAPH_ALGO
          public graphlab::algo_to_gas<std::pair<double, double>, PRA_MSG, pagerank_vertex_data, double>,
#endif
          public graphlab::IS_POD_TYPE {

    double message_value;
    bool scatter_active = false;
    double reset_prob = RESET_PROB;
    double delta_threshold = DELTA_TOL;

public:
#ifdef GRAPH_ALGO

    void algo_to_gas_message_convert(double *graphlab_mValues, graphlab::dense_bitset *has_message,
                                     int vertex_count, int edge_count) override {

        if (this->get_algo_client_ptr() == nullptr || graphlab_mValues == nullptr || has_message == nullptr) {
            return;
        }

        PRA_MSG *algo_mValues = this->get_algo_client_ptr()->mValues;

        if (algo_mValues == nullptr) return;

        for (int i = 0; i < vertex_count; i++) {
            if (algo_mValues[i].destVId != -1) {
                graphlab_mValues[i] += algo_mValues[i].rank;
                has_message->set_bit(i);
            }
        }
    }

    void gas_to_algo_message_convert(double *graphlab_mValues, graphlab::dense_bitset *has_message,
                                     int vertex_count, int edge_count) override {

        if (this->get_algo_client_ptr() == nullptr || graphlab_mValues == nullptr || has_message == nullptr) {
            return;
        }

        PRA_MSG *algo_mValues = this->get_algo_client_ptr()->mValues;

        if (algo_mValues == nullptr) return;

        for (int i = 0; i < vertex_count; i++) {
            if (has_message->get(i)) {
                algo_mValues[i].destVId = i;
                algo_mValues[i].rank = graphlab_mValues[i];
            }
        }
    }

    void algo_to_gas_value_convert(pagerank_vertex_data *graphlab_value, std::pair<double, double> *algo_value) override {
        graphlab_value->rank = algo_value->first;
        graphlab_value->delta = algo_value->second;
    }

    void gas_to_algo_value_convert(pagerank_vertex_data *graphlab_value, std::pair<double, double> *algo_value) override {
        algo_value->first = graphlab_value->rank;
        algo_value->second = graphlab_value->delta;
    }

#endif

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
