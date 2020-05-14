project(sssp_graph_algo)

#graph algo
add_executable(sssp_graph_algo
        sssp.cpp
        sssp.h
        )

target_link_libraries(sssp_graph_algo
        graphlab
        srv_UtilClient)

add_dependencies(sssp_graph_algo
        graphlab)

requires_core_deps(sssp_graph_algo)

