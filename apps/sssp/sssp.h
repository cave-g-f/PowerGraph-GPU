//
// Created by kdy on 2020/4/20.
//

#ifndef GRAPHLAB_SSSP_H
#define GRAPHLAB_SSSP_H

#include <vector>
#include <string>
#include <fstream>

#ifdef GRAPH_ALGO

#include "sssp_to_gas.h"

#endif

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



/**
 * \brief The distance associated with the edge.
 */
struct edge_data : graphlab::IS_POD_TYPE {
    distance_type dist;

    edge_data(distance_type dist = 1) : dist(dist) {}
}; // end of edge data


/**
 * \brief The graph type encodes the distances between vertices and
 * edges
 */
typedef graphlab::distributed_graph<vertex_data, edge_data> graph_type;


/**
 * \brief Get the other vertex in the edge.
 */
inline graph_type::vertex_type
get_other_vertex(const graph_type::edge_type &edge,
                 const graph_type::vertex_type &vertex) {
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
#ifdef GRAPH_ALGO

template<typename algo_value_type, typename algo_message_type>
#endif
class sssp :
        public graphlab::ivertex_program<graph_type,
                graphlab::empty,
                min_distance_type>,
        public graphlab::IS_POD_TYPE {
    distance_type min_dist;
    bool changed;
private:
#ifdef GRAPH_ALGO
    sssp_to_gas<algo_value_type, algo_message_type, min_distance_type> *algo_ptr = nullptr;
#endif

public:
#ifdef GRAPH_ALGO

    void set_sssp_to_gas_ptr(sssp_to_gas<algo_value_type, algo_message_type, min_distance_type> *ptr) {
        this->algo_ptr = ptr;
    }

    sssp_to_gas<algo_value_type, algo_message_type, min_distance_type> *get_sssp_to_gas_ptr() {
        return this->algo_ptr;
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
        changed = false;
        if (vertex.data().dist > min_dist) {
            changed = true;
            vertex.data().dist = min_dist;
        }
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
        const vertex_type other = get_other_vertex(edge, vertex);
        distance_type newd = vertex.data().dist + edge.data().dist;
        if (other.data().dist > newd) {
            const min_distance_type msg(newd);
            context.signal(other, msg);
        }
    } // end of scatter

}; // end of shortest path vertex program

#endif //GRAPHLAB_SSSP_H
