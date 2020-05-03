//
// Created by kdy on 2020/4/21.
//

#include <graphlab/graph_algo/graph_algo_connect_gas.h>

namespace graphlab {

    template<typename algo_value_type, typename algo_message_type, typename graphlab_value_type, typename graphlab_message_type>
    UtilClient<algo_value_type, algo_message_type> *
    algo_to_gas<algo_value_type, algo_message_type, graphlab_value_type, graphlab_message_type>::get_algo_client_ptr() {
        return this->client_ptr;
    }

    template<typename algo_value_type, typename algo_message_type, typename graphlab_value_type, typename graphlab_message_type>
    UtilClient<algo_value_type, algo_message_type> *
    algo_to_gas<algo_value_type, algo_message_type, graphlab_value_type, graphlab_message_type>::set_algo_client_ptr(
            UtilClient<algo_value_type, algo_message_type> *ptr) {
        if (ptr != nullptr) this->client_ptr = ptr;
    }

    template<typename algo_value_type, typename algo_message_type, typename graphlab_value_type, typename graphlab_message_type>
    void
    algo_to_gas<algo_value_type, algo_message_type, graphlab_value_type, graphlab_message_type>::request_for_MSGApply() {
        this->client_ptr->requestMSGApply();
    }

    template<typename algo_value_type, typename algo_message_type, typename graphlab_value_type, typename graphlab_message_type>
    void
    algo_to_gas<algo_value_type, algo_message_type, graphlab_value_type, graphlab_message_type>::request_for_MSGMerge() {
        this->client_ptr->requestMSGMerge();
    }

}





