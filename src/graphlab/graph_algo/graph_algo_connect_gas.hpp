//
// Created by kdy on 2020/4/20.
//

#ifndef GRAPHLAB_GRAPH_ALGO_CONNECT_GAS_HPP
#define GRAPHLAB_GRAPH_ALGO_CONNECT_GAS_HPP

#include <graphlab/util/dense_bitset.hpp>
#include "../../../Graph_Algo/srv/UtilClient.h"

namespace graphlab {
    template<typename algo_value_type, typename algo_message_type, typename graphlab_value_type, typename graphlab_message_type>
    class algo_to_gas {
    public:
        UtilClient<algo_value_type, algo_message_type> *get_algo_client_ptr(){
            return this->client_ptr;
        }

        UtilClient<algo_value_type, algo_message_type> *
        set_algo_client_ptr(UtilClient<algo_value_type, algo_message_type> *ptr){
            if (ptr != nullptr) this->client_ptr = ptr;
        }

        virtual void algo_to_gas_message_convert(graphlab_message_type *graphlab_mValues,
                                                 graphlab::dense_bitset *has_message, int vertex_count,
                                                 int edge_count) = 0;

        virtual void gas_to_algo_message_convert(graphlab_message_type *graphlab_mValues,
                                                 graphlab::dense_bitset *has_message, int vertex_count,
                                                 int edge_count) = 0;

        virtual void algo_to_gas_value_convert(graphlab_value_type *graphlab_value, algo_value_type *algo_value) = 0;

        virtual void gas_to_algo_value_convert(graphlab_value_type *graphlab_value, algo_value_type *algo_value) = 0;


        void request_for_MSGApply(){
            this->client_ptr->requestMSGApply();
        }

        void request_for_MSGMerge(){
            this->client_ptr->requestMSGMerge();
        }

    private:
        UtilClient<algo_value_type, algo_message_type> *client_ptr = nullptr;
    };

}

#endif //GRAPHLAB_GRAPH_ALGO_CONNECT_GAS_HPP
