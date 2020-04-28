project(sssp_graph_algo)

#power graph
#add_graphlab_executable(label_propagation label_propagation.cpp)

#graph algo
add_executable(sssp_graph_algo
        sssp.cpp
        sssp.h
        )

target_link_libraries(sssp_graph_algo
        graphlab
        algo_to_gas
        srv_UtilClient)

add_dependencies(sssp_graph_algo
        graphlab)

requires_core_deps(sssp_graph_algo)

