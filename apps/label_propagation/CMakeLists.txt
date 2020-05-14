project(label_propagation)

add_executable(lpa_graph_algo
        label_propagation.cpp
        label_propagation.h
        )

target_link_libraries(lpa_graph_algo
        graphlab
        srv_UtilClient)

add_dependencies(lpa_graph_algo
        graphlab)

requires_core_deps(lpa_graph_algo)
