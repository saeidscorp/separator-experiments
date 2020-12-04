
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

int main() {

    auto linear_gen = new gen::LinearGenerator<true>(400);
    auto rand_g = linear_gen->generate();
    std::cout << rand_g->dotString() << std::endl;

    auto nodes_ids = rand_g->getNodeIds();
    auto n1 = rand_g->getNode(nodes_ids[3]).value();
    auto n2 = rand_g->getNode(nodes_ids[nodes_ids.size() - 3]).value();

    std::cout << "# Oracle:" << std::endl;
    auto oracle = new alg::Oracle(rand_g);
    auto result = oracle->query(n1, n2);
    util::print_path(n1, n2, result);

    std::cout << std::endl;

    std::cout << "# Linear Separator:" << std::endl;
    auto linear_sep = new alg::LinearSeparator(rand_g);
    result = linear_sep->query(n1, n2);
    util::print_path(n1, n2, result);

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

