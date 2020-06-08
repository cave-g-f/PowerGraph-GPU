//
// Created by kdy on 2020/4/29.
//

#ifndef GRAPHLAB_LABEL_PROPAGATION_H
#define GRAPHLAB_LABEL_PROPAGATION_H

#include <vector>
#include <graphlab.hpp>

#include "../../Graph_Algo/algo/LabelPropagation/LabelPropagation.h"

struct label_counter {
    std::map<int, int> label_count;

    label_counter() = default;

    label_counter &operator+=(const label_counter &other) {
        for (const auto &iter : other.label_count) {
            if (label_count.find(iter.first) != label_count.end()) {
                label_count[iter.first] += iter.second;
            } else {
                label_count[iter.first] = 1;
            }
        }
        return *this;
    }

    void save(graphlab::oarchive &oarc) const {
        oarc << label_count;
    }

    void load(graphlab::iarchive &iarc) {
        iarc >> label_count;
    }
};

#ifndef GRAPH_ALGO
// The vertex data is its label
typedef int vertex_data_type;
#endif

#ifdef GRAPH_ALGO
typedef graphlab::distributed_graph<int, int> lpa_graph_type;
#else
// The graph type is determined by the vertex and edge data types
typedef graphlab::distributed_graph<int, int> lpa_graph_type;
#endif

class labelpropagation :
        public graphlab::ivertex_program<lpa_graph_type, graphlab::empty, label_counter>,
#ifdef GRAPH_ALGO
        public graphlab::algo_to_gas<LPA_Value, LPA_MSG, int, label_counter>,
#endif
        public graphlab::IS_POD_TYPE {
    bool changed;
    int label;

public:
#ifdef GRAPH_ALGO

    void
    algo_to_gas_message_convert(label_counter *graphlab_mValues, graphlab::dense_bitset *has_message, int vertex_count,
                                int edge_count) override {

        if (this->get_algo_client_ptr() == nullptr || graphlab_mValues == nullptr || has_message == nullptr) {
            return;
        }

        LPA_MSG *algo_mValues = this->get_algo_client_ptr()->mValues;

        if (algo_mValues == nullptr) return;

        for (int i = 0; i < edge_count; i++) {
            label_counter label_count;
            LPA_MSG msg = algo_mValues[i];

            label_count.label_count[msg.label] = 1;
            graphlab_mValues[msg.destVId] += label_count;

            has_message->set_bit(msg.destVId);
        }
    }

    void
    gas_to_algo_message_convert(label_counter *graphlab_mValues, graphlab::dense_bitset *has_message, int vertex_count,
                                int edge_count) override {

        if (this->get_algo_client_ptr() == nullptr || graphlab_mValues == nullptr || has_message == nullptr) {
            return;
        }

        LPA_MSG *algo_mValues = this->get_algo_client_ptr()->mValues;

        if (algo_mValues == nullptr) return;

        for (int i = 0; i < std::max(vertex_count, edge_count); i++)
            algo_mValues[i].destVId = -1;

        for (int i = 0; i < vertex_count; i++) {
            if (has_message->get(i)) {
                label_counter msg = graphlab_mValues[i];
                int maxCount = 0;

                algo_mValues[i].destVId = i;
                for (auto iter : msg.label_count) {
                    if (iter.second >= maxCount) {
                        algo_mValues[i].label = iter.first;
                        maxCount = iter.second;
                    }
                }
            }
        }
    }

    void algo_to_gas_value_convert(int *graphlab_value, LPA_Value *algo_value) override {
        *graphlab_value = algo_value->label;
    }

    void gas_to_algo_value_convert(int *graphlab_value, LPA_Value *algo_value) override {
        algo_value->label = *graphlab_value;
    }


#endif

    void init(icontext_type &context, const vertex_type &vertex, const label_counter &msg) {
        int maxCount = 0;

        // Figure out which label of the vertex's neighbors' labels is most common
        for (auto iter : msg.label_count) {
            if (iter.second >= maxCount) {
                maxCount = iter.second;
                label = iter.first;
            }
        }

    }

    edge_dir_type gather_edges(icontext_type &context, const vertex_type &vertex) const {
        return graphlab::NO_EDGES;
    }

    void apply(icontext_type &context, vertex_type &vertex, const graphlab::empty &empty) {

        vertex.data() = label;
    }

    edge_dir_type scatter_edges(icontext_type &context, const vertex_type &vertex) const {
        return graphlab::OUT_EDGES;
    }

    void scatter(icontext_type &context, const vertex_type &vertex, edge_type &edge) const {
        vertex_type other = edge.target();
        label_counter msg;

        msg.label_count[vertex.data()] = 1;
        context.signal(other, msg);
    }
};

#endif //GRAPHLAB_LABEL_PROPAGATION_H
