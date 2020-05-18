#include "label_propagation.h"

bool line_parser(lpa_graph_type &graph, const std::string &filename, const std::string &textline) {
    std::stringstream strm(textline);
    graphlab::vertex_id_type src_vid, dest_vid;
    float weight;
    // first entry in the line is a vertex ID
    strm >> src_vid;
    strm >> dest_vid;
    strm >> weight;
    // insert this vertex with its label
#ifdef GRAPH_ALGO
    graph.add_edge(src_vid, dest_vid, weight);
#else
    graph.add_vertex(src_vid, src_vid);
    graph.add_edge(src_vid, dest_vid, weight);
#endif

    return true;
}

struct labelpropagation_writer {
    std::string save_vertex(lpa_graph_type::vertex_type v) {
        std::stringstream strm;
        strm << v.id() << "\t" << v.data() << "\n";
        return strm.str();
    }

    std::string save_edge(lpa_graph_type::edge_type e) { return ""; }
};


int main(int argc, char **argv) {
    // Initialize control plain using mpi
    graphlab::mpi_tools::init(argc, argv);
    graphlab::distributed_control dc;
    global_logger().set_log_level(LOG_INFO);

    // Parse command line options -----------------------------------------------
    graphlab::command_line_options clopts("Label Propagation algorithm.");
    std::string graph_dir;
    std::string execution_type = "synchronous";
    int max_iterations = 0;
    clopts.attach_option("graph", graph_dir, "The graph file. Required ");
    clopts.add_positional("graph");
    clopts.attach_option("execution", execution_type, "Execution type (synchronous or asynchronous)");

    clopts.attach_option("max_iterations", max_iterations, "max_iterations of the lpa");

    std::string saveprefix;
    clopts.attach_option("saveprefix", saveprefix,
                         "If set, will save the resultant pagerank to a "
                         "sequence of files with prefix saveprefix");

    if (!clopts.parse(argc, argv)) {
        dc.cout() << "Error in parsing command line arguments." << std::endl;
        return EXIT_FAILURE;
    }
    if (graph_dir == "") {
        dc.cout() << "Graph not specified. Cannot continue";
        return EXIT_FAILURE;
    }

    clopts.get_engine_args().set_option("max_iterations", max_iterations);

    // Build the graph ----------------------------------------------------------
    lpa_graph_type graph(dc);
    dc.cout() << "Loading graph using line parser" << std::endl;
    graph.load(graph_dir, line_parser);
    // must call finalize before querying the graph
    graph.finalize();

    dc.cout() << "#vertices: " << graph.num_vertices() << " #edges:" << graph.num_edges() << std::endl;

#ifndef GRAPH_ALGO
    graphlab::omni_engine<labelpropagation> engine(dc, graph, execution_type, clopts);

    for (int i = 0; i < graph.num_vertices(); i++) {
        label_counter msg;
        msg.label_count[i] = 1;
        engine.signal(i, msg);
    }
    engine.start();
#endif

#ifdef GRAPH_ALGO
//  use local graph info to init client-server
    int local_vCount = graph.get_local_graph().num_vertices();
    int local_eCount = graph.get_local_graph().num_edges();

    dc.cout() << "#local vertices:  " << local_vCount << std::endl
              << "#local edges:     " << local_eCount << std::endl;

    Vertex *vSet = new Vertex[local_vCount];
    Edge *eSet = new Edge[local_eCount];
    bool *filteredV = new bool[local_vCount];
    int *timestamp = new int[local_vCount];
    LPA_Value *vValues = new LPA_Value [local_vCount];
    int initVSet = 0;

    //init vSet
    for (int i = 0; i < local_vCount; i++) {
        vSet[i].outDegree = graph.l_vertex(i).global_num_out_edges();
        vSet[i].inDegree = graph.l_vertex(i).global_num_in_edges();
        vSet[i].isMaster = graph.l_is_master(i);

        //all vertex should be always active
        vSet[i].isActive = true;
    }

    //init eSet
    for (int i = 0; i < local_eCount; i++) {
        eSet[i].src = graph.get_local_graph().get_edge_source(i);
        eSet[i].dst = graph.get_local_graph().get_edge_target(i);
        eSet[i].weight = graph.get_local_graph().get_edge_data(i);
    }

    //connect client
    auto client = UtilClient<LPA_Value, LPA_MSG>(local_vCount, local_eCount, 1);
    int chk = 0;
    chk = client.connect();
    if (chk == -1) {
        std::cout << "Cannot establish the connection with server correctly" << std::endl;
    }

    //init vValues
    for(int i = 0; i < local_vCount; i++)
    {
        vValues[i].label = graph.global_vid(i);
    }

    chk = client.transfer(vValues, vSet, eSet, &initVSet, filteredV, timestamp);
    if (chk == -1) {
        std::cout << "Parameter illegal" << std::endl;
    }

    client.requestMSGMerge();

    std::cout << "connect successful" << std::endl;

    //engine start
    graphlab::synchronous_engine_algo<labelpropagation> engine(dc, graph, clopts);

    engine.get_vertex_program().set_algo_client_ptr(&client);

    engine.start();

    //copy vertex value back to vertex data
    for (int i = 0; i < local_vCount; i++) {
        graph.l_vertex(i).data() = client.vValues[i].label;
    }

    client.disconnect();
    client.shutdown();

    delete [] vValues;
    delete [] eSet;
    delete [] vSet;
    delete [] filteredV;
    delete [] timestamp;
#endif

    const float runtime = engine.elapsed_seconds();
    dc.cout() << "Finished Running engine in " << runtime << " seconds." << std::endl;

    if (saveprefix != "") {
        graph.save(saveprefix, labelpropagation_writer(),
                   false,  // do not gzip
                   true,   //save vertices
                   true,
                   1); // do not save edges
    }


    graphlab::mpi_tools::finalize();
    return EXIT_SUCCESS;
}
