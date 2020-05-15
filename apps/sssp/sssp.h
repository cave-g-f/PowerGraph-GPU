//
// Created by kdy on 2020/4/20.
//

#ifndef GRAPHLAB_SSSP_H
#define GRAPHLAB_SSSP_H

#include <vector>
#include <string>
#include <fstream>

#include <graphlab.hpp>

/**
 * \brief The type used to measure distances in the graph.
 */
typedef float distance_type;

/**
 * \brief The current distance of the vertex.
 */
struct vertex_data : graphlab::IS_POD_TYPE {
    distance_type dist;

    vertex_data(distance_type dist = std::numeric_limits<distance_type>::max()) :
            dist(dist) {}
}; // end of vertex data


#ifndef GRAPH_ALGO
/**
 * \brief The distance associated with the edge.
 */
struct edge_data : graphlab::IS_POD_TYPE {
    distance_type dist;

    edge_data(distance_type dist = 1) : dist(dist) {}
}; // end of edge data
#endif


/**
 * \brief The graph type encodes the distances between vertices and
 * edges
 */
#ifdef GRAPH_ALGO
typedef graphlab::distributed_graph<double, double> sssp_graph_type;
#else
typedef graphlab::distributed_graph<vertex_data, edge_data> sssp_graph_type;
#endif


/**
 * \brief Get the other vertex in the edge.
 */
inline sssp_graph_type::vertex_type
get_other_vertex(const sssp_graph_type::edge_type &edge,
                 const sssp_graph_type::vertex_type &vertex) {
    return vertex.id() == edge.source().id() ? edge.target() : edge.source();
}

/**
 * \brief This class is used as the gather type.
 */
struct min_distance_type : graphlab::IS_POD_TYPE {
    distance_type dist;

    min_distance_type(distance_type dist =
    std::numeric_limits<distance_type>::max()) : dist(dist) {}

    min_distance_type &operator+=(const min_distance_type &other) {
        dist = std::min(dist, other.dist);
        return *this;
    }
};

/**
 * \brief The single source shortest path vertex program.
 */
class sssp :
        public graphlab::ivertex_program<sssp_graph_type,
                graphlab::empty,
                min_distance_type>,
#ifdef GRAPH_ALGO
        public graphlab::algo_to_gas<double, double, double, min_distance_type>,
#endif
        public graphlab::IS_POD_TYPE {
    distance_type min_dist;
    bool changed;

public:
#ifdef GRAPH_ALGO

    void algo_to_gas_message_convert(min_distance_type *graphlab_mValues, graphlab::dense_bitset *has_message,
                                     int vertex_count, int edge_count) override {

        if (this->get_algo_client_ptr() == nullptr || graphlab_mValues == nullptr || has_message == nullptr) {
            return;
        }

        double *algo_mValues = this->get_algo_client_ptr()->mValues;

        if (algo_mValues == nullptr) return;

        for (int i = 0; i < vertex_count; i++) {
            if (algo_mValues[i] != INVALID_MASSAGE) {
                graphlab_mValues[i] += min_distance_type(algo_mValues[i]);
                has_message->set_bit(i);
            }
        }
    }

    void gas_to_algo_message_convert(min_distance_type *graphlab_mValues, graphlab::dense_bitset *has_message,
                                     int vertex_count, int edge_count) override {

        if (this->get_algo_client_ptr() == nullptr || graphlab_mValues == nullptr || has_message == nullptr) {
            return;
        }

        double *algo_mValues = this->get_algo_client_ptr()->mValues;

        if (algo_mValues == nullptr) return;

        for (int i = 0; i < vertex_count; i++) {
            if (has_message->get(i)) {
                algo_mValues[i] = graphlab_mValues[i].dist;
            }
        }
    }

    void algo_to_gas_value_convert(double *graphlab_value, double *algo_value) override {
        *graphlab_value = *algo_value;
    }

    void gas_to_algo_value_convert(double *graphlab_value, double *algo_value) override{
        *algo_value = *graphlab_value;
    }

#endif

    void init(icontext_type &context, const vertex_type &vertex,
              const min_distance_type &msg) {
        min_dist = msg.dist;
    }

    /**
     * \brief We use the messaging model to compute the SSSP update
     */
    edge_dir_type gather_edges(icontext_type &context,
                               const vertex_type &vertex) const {
        return graphlab::NO_EDGES;
    }; // end of gather_edges


    /**
     * \brief If the distance is smaller then update
     */
    void apply(icontext_type &context, vertex_type &vertex,
               const graphlab::empty &empty) {
#ifndef GRAPH_ALGO
        changed = false;
        if (vertex.data().dist > min_dist) {
            changed = true;
            vertex.data().dist = min_dist;
        }
#endif
    }

    /**
     * \brief Determine if SSSP should run on all edges or just in edges
     */
    edge_dir_type scatter_edges(icontext_type &context,
                                const vertex_type &vertex) const {
        if (changed)
            return graphlab::OUT_EDGES;
        else return graphlab::NO_EDGES;
    }; // end of scatter_edges

    /**
     * \brief The scatter function just signal adjacent pages
     */
    void scatter(icontext_type &context, const vertex_type &vertex,
                 edge_type &edge) const {
#ifndef GRAPH_ALGO
        const vertex_type other = get_other_vertex(edge, vertex);
        distance_type newd = vertex.data().dist + edge.data().dist;
        if (other.data().dist > newd) {
            const min_distance_type msg(newd);
            context.signal(other, msg);
        }
#endif
    } // end of scatter

}; // end of shortest path vertex program

#endif //GRAPHLAB_SSSP_H
