//
// Created by kdy on 2020/4/20.
//

#ifndef GRAPHLAB_GRAPH_ALGO_CONNECT_GAS_H
#define GRAPHLAB_GRAPH_ALGO_CONNECT_GAS_H

#include <graphlab/util/dense_bitset.hpp>
#include "../../../Graph_Algo/srv/UtilClient.h"

namespace graphlab {
    template<typename algo_value_type, typename algo_message_type, typename graphlab_message_type>
    class algo_to_gas {
    public:
        algo_to_gas(UtilClient<algo_value_type, algo_message_type> *ptr) {
            this->client_ptr = ptr;
        }

        UtilClient<algo_value_type, algo_message_type> *get_algo_client_ptr() {
            return this->client_ptr;
        }

        virtual void algo_to_gas_message_merge(algo_message_type *algo_mValues, graphlab_message_type *graphlab_mValues,
                                               graphlab::dense_bitset *has_message) = 0;

    private:
        UtilClient<algo_value_type, algo_message_type> *client_ptr = nullptr;
    };
}

#endif //GRAPHLAB_GRAPH_ALGO_CONNECT_GAS_H
