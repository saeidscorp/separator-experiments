
#include <iostream>
#include <chrono>

#include "model/model.hpp"
#include "gen/LinearGenerator.hpp"
#include "alg/LinearSeparator.h"
#include "alg/Oracle.hpp"
#include "util/stlutils.hpp"
#include "util/printutils.hpp"

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

    int n = 10;

    auto graph_size = get_cmd_option(argc, argv, "-n");
    if (graph_size)
        n = std::stoi(graph_size.value());

    auto linear_gen = new gen::LinearGenerator<true>(n);
    auto rand_g = linear_gen->generate();
    auto oracle = new alg::Oracle(rand_g);
    std::cout << "# Linear Separator:" << std::endl;
    auto linear_sep = new alg::LinearSeparator(rand_g);
    std::cout << "Linear separator used " << linear_sep->preprocessing_queries() << " queries from the oracle"
              << std::endl;
    std::cout << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    auto similarity = linear_sep->similarity();
    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << ":: computing similarity took " << elapsed.count() << " seconds ::" << std::endl;

    std::cout << "MSE of linear separator-induced graph to the reference graph is: " << similarity << std::endl;

    start = std::chrono::high_resolution_clock::now();
    auto random_from_g = random_weights(rand_g);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << ":: randomizing weights took " << elapsed.count() << " seconds ::" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    similarity = oracle->similarity(random_from_g);
    elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << ":: computing similarity took " << elapsed.count() << " seconds" << std::endl;

    std::cout << "MSE of reference graph but with random weights to the reference itself is: "
              << similarity << std::endl;

    return 0;
}

