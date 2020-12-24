
#include <iostream>
#include <chrono>

#include "model/model.hpp"
#include "gen/LinearGenerator.hpp"
#include "alg/LinearSeparator.h"
#include "gen/TreeGenerator.hpp"
#include "alg/Oracle.hpp"
#include "util/stlutils.hpp"
#include "util/printutils.hpp"
#include "alg/CentroidDecomposition.h"

template<bool bidirectional_graph = true>
model::Graph<bidirectional_graph> *random_weights(model::Graph<bidirectional_graph> *g) {
    auto g_copy = new model::Graph<bidirectional_graph>(*g);
    auto nodes = g->getNodes();
    auto start_index = 0ul, end_index = nodes.size() - 1;
    for (auto i = start_index; i < end_index; ++i)
        for (auto j = start_index; j < end_index; ++j) {
            auto from = nodes[i], to = nodes[j];
            auto eta = Random::get(0., 1.);
            auto our_edge = g_copy->getEdgeBetween(from->getId(), to->getId());
            if (our_edge)
                our_edge.value()->setEta(eta);
        }
    return g_copy;
}

std::optional<std::string> get_cmd_option(const char ** begin, const char ** end, const std::string & option)
{
    const char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
        return *itr;
    return {};
}

std::optional<std::string> get_cmd_option(const int argc, const char **argv, const std::string & option)
{
    return get_cmd_option(argv, argv + argc, option);
}

int main(int argc, const char **argv) {

    gen::TreeGenerator tree_gen{16, 2, 4};
//    gen::LinearGenerator<true> linear_gen{4};
    auto graph = tree_gen.generate();
//    auto graph = linear_gen.generate();

    auto start = std::chrono::high_resolution_clock::now();
    alg::CentroidDecomposition centroid_decomposition{graph};
    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << ":: tree decomposing took " << elapsed.count() << " seconds ::" << std::endl;

    auto tree = centroid_decomposition.toGraph();

    util::visualize_graph(graph);
    util::visualize_graph(tree);

    return 0;
}

