project(sssp_graph_algo)

#power graph
#add_graphlab_executable(label_propagation label_propagation.cpp)

#graph algo
add_executable(sssp_graph_algo
        sssp.cpp
        sssp.h
        sssp_to_gas.h
        sssp_to_gas.cpp
        sssp_to_gas_impl.cpp)

target_link_libraries(sssp_graph_algo
        graphlab
        srv_UtilClient)

add_dependencies(sssp_graph_algo
        graphlab)

requires_core_deps(sssp_graph_algo)

