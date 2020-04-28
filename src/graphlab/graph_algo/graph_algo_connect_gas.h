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
        UtilClient<algo_value_type, algo_message_type> *get_algo_client_ptr();

        UtilClient<algo_value_type, algo_message_type> *
        set_algo_client_ptr(UtilClient<algo_value_type, algo_message_type> *ptr);

        virtual void algo_to_gas_message_convert(graphlab_message_type *graphlab_mValues,
                                                 graphlab::dense_bitset *has_message) = 0;

        virtual void gas_to_algo_message_convert(graphlab_message_type *graphlab_mValues,
                                                 graphlab::dense_bitset *has_message) = 0;

        void request_for_MSGApply();

        void request_for_MSGMerge();

        int get_local_vertex_num() const;

        void set_local_vertex_num(int _local_vertex_num);

    private:
        int local_vertex_num;
        UtilClient<algo_value_type, algo_message_type> *client_ptr = nullptr;
    };

}

#endif //GRAPHLAB_GRAPH_ALGO_CONNECT_GAS_H
