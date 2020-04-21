//
// Created by kdy on 2020/4/20.
//

#ifndef GRAPHLAB_SSSP_TO_GAS_H
#define GRAPHLAB_SSSP_TO_GAS_H

#include <graphlab/graph_algo/graph_algo_connect_gas.hpp>

template<typename algo_value_type, typename algo_message_type, typename graphlab_message_type>
class sssp_to_gas : public graphlab::algo_to_gas<algo_value_type, algo_message_type, graphlab_message_type> {
public:
    sssp_to_gas(UtilClient<algo_value_type, algo_message_type> *ptr)
            : graphlab::algo_to_gas<algo_value_type, algo_message_type, graphlab_message_type>(ptr) {

    };

    void algo_to_gas_message_merge(algo_message_type *algo_mValues, graphlab_message_type *graphlab_mValues,
                                   graphlab::dense_bitset *has_message);

};

#endif //GRAPHLAB_SSSP_TO_GAS_H
