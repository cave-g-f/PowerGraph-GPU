project(pagerank_graph_algo)

#graph algo
add_executable(pagerank_graph_algo
        pagerank.cpp
        pagerank.h
        )

target_link_libraries(pagerank_graph_algo
        graphlab
        algo_to_gas
        srv_UtilClient)

add_dependencies(pagerank_graph_algo
        graphlab)

requires_core_deps(pagerank_graph_algo)

