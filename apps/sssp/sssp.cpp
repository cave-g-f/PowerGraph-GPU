//created by cave-g-f 2020/4/2

#include "sssp.h"

/**
 * \brief Use directed or undireced edges.
 */
bool DIRECTED_SSSP = true;


bool line_parser(sssp_graph_type &graph, const std::string &filename, const std::string &textline) {
    std::stringstream strm(textline);
    graphlab::vertex_id_type src_vid, dest_vid;
    float weight;

    strm >> src_vid;
    strm >> dest_vid;
    strm >> weight;

    graph.add_edge(src_vid, dest_vid, weight);
    return true;
}

/**
 * \brief We want to save the final graph so we define a write which will be
 * used in graph.save("path/prefix", pagerank_writer()) to save the graph.
 */
struct shortest_path_writer {
    std::string save_vertex(const sssp_graph_type::vertex_type &vtx) {
        std::stringstream strm;

        strm << vtx.id() << "\t" << vtx.data() << "\n";
        return strm.str();
    }

    std::string save_edge(sssp_graph_type::edge_type e) { return ""; }
}; // end of shortest_path_writer



int main(int argc, char **argv) {
    // Initialize control plain using mpi
    graphlab::mpi_tools::init(argc, argv);
    graphlab::distributed_control dc;
    global_logger().set_log_level(LOG_INFO);

    // Parse command line options -----------------------------------------------
    graphlab::command_line_options
            clopts("Single Source Shortest Path Algorithm.");
    std::string graph_dir;
    std::string exec_type = "algo";
    int max_iterations = 0;

    int source;

    clopts.attach_option("graph", graph_dir,
                         "The graph file.  If none is provided "
                         "then a toy graph will be created");
    clopts.add_positional("graph");
    clopts.attach_option("source", source,
                         "The source vertices");
    clopts.add_positional("source");
    clopts.attach_option("engine", exec_type,
                         "exec engine");

    std::string saveprefix = "";
    clopts.attach_option("saveprefix", saveprefix,
                         "If set, will save the resultant pagerank to a "
                         "sequence of files with prefix saveprefix");

    clopts.add_positional("max_iterations");
    clopts.attach_option("max_iterations", max_iterations, "max_iterations of the sssp");

    if (!clopts.parse(argc, argv)) {
        dc.cout() << "Error in parsing command line arguments." << std::endl;
        return EXIT_FAILURE;
    }

    if (max_iterations) clopts.get_engine_args().set_option("max_iterations", max_iterations);

    // Build the graph ----------------------------------------------------------
    sssp_graph_type graph(dc);
    dc.cout() << "Loading graph using line parser" << std::endl;
    graph.load(graph_dir, line_parser);
    // must call finalize before querying the graph
    graph.finalize();


    dc.cout() << "#vertices:  " << graph.num_vertices() << std::endl
              << "#edges:     " << graph.num_edges() << std::endl;

    // Running The Engine -------------------------------------------------------

#ifndef GRAPH_ALGO
    graphlab::omni_engine<sssp> engine(dc, graph, exec_type, clopts);

    // Signal all the vertices in the source set
    engine.signal(source, min_distance_type(0));
    engine.start();
#endif

#ifdef GRAPH_ALGO
//  use local graph info to init client-server
    int local_vCount = graph.get_local_graph().num_vertices();
    int local_eCount = graph.get_local_graph().num_edges();

    std::cout << "#local vertices:  " << local_vCount << std::endl
              << "#local edges:     " << local_eCount << std::endl;

    std::vector<Vertex> vSet;
    std::vector<Edge> eSet;
    std::vector<double> vValues;
    bool filteredV[local_vCount];
    int timestamp[local_vCount];
    int initVSet = -1;

    vSet.resize(local_vCount);
    eSet.resize(local_eCount);
    vValues.resize(local_vCount);

    //init vSet
    if(graph.contains_vertex(source)){
        initVSet = graph.local_vid(source);
        vSet.at(graph.local_vid(source)).initVIndex = 0;
    }
    for (int i = 0; i < local_vCount; i++) {
        vSet.at(i).outDegree = graph.l_vertex(i).global_num_out_edges();
        vSet.at(i).inDegree = graph.l_vertex(i).global_num_in_edges();
        vSet.at(i).isMaster = graph.l_is_master(i);
    }

    //init vValuesSet
    for (int i = 0; i < local_vCount; i++) {
        vValues.at(i) = INT32_MAX  >> 1;
    }

    //init eSet
    for (int i = 0; i < local_eCount; i++) {
        eSet.at(i).src = graph.get_local_graph().get_edge_source(i);
        eSet.at(i).dst = graph.get_local_graph().get_edge_target(i);
        eSet.at(i).weight = graph.get_local_graph().get_edge_data(i);
    }

    //connect client
    auto client = UtilClient<double, double>(local_vCount, local_eCount, 1);
    int chk = 0;
    chk = client.connect();
    if (chk == -1) {
        std::cout << "Cannot establish the connection with server correctly" << std::endl;
    }

    chk = client.transfer(&vValues[0], &vSet[0], &eSet[0], &initVSet, filteredV, timestamp);
    if (chk == -1) {
        std::cout << "Parameter illegal" << std::endl;
    }

    //message init
    for (int i = 0; i < local_vCount; i++) {
        client.mValues[i] = INVALID_MASSAGE;
    }
    if (graph.is_master(source)) {
        auto local_vid = graph.vertex(source).local_id();
        std::cout << "local_vid:" << local_vid << std::endl;
        client.mValues[local_vid] = 0;
    }

    std::cout << "connect successful" << std::endl;

    //engine start
    graphlab::synchronous_engine_algo<sssp> engine(dc, graph, clopts);

    engine.get_vertex_program().set_algo_client_ptr(&client);

    engine.start();

    //copy vertex value back to vertex data
    for(int i = 0; i < local_vCount; i++)
    {
        graph.l_vertex(i).data() = client.vValues[i];
    }

    client.disconnect();
    client.shutdown();
#endif

    const float runtime = engine.elapsed_seconds();
    dc.cout() << "Finished Running engine in " << runtime
              << " seconds." << std::endl;


    // Save the final graph -----------------------------------------------------
    if (saveprefix != "") {
        graph.save(saveprefix, shortest_path_writer(),
                   false,    // do not gzip
                   true,     // save vertices
                   false, 1);   // do not save edges
    }

    // Tear-down communication layer and quit -----------------------------------
    graphlab::mpi_tools::finalize();
    return EXIT_SUCCESS;
} // End of main


// We render this entire program in the documentation



