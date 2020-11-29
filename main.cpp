
#include <iostream>

#include "model/model.hpp"
#include "gen/LinearGenerator.hpp"
#include "alg/LinearSeparator.h"
#include "alg/Oracle.hpp"
#include "util/stlutils.hpp"

int main() {
//    std::cout << "Simple benchmark for randomized node selection" << std::endl;

//    auto g = new model::Graph;
//
//    auto n1 = g->createNode(10, 20, "node_1");
//    auto n2 = g->createNode(0, -10, "node_2");
//
//    g->connect(n1, n2, 100, 5, "edge_1");

    auto linear_gen = new gen::LinearGenerator<true>;
    auto rand_g = linear_gen->generate();
    std::cout << rand_g->dotString() << std::endl;
    auto &g = rand_g;
    auto nodes_ids = rand_g->getNodeIds();
    auto n1 = rand_g->getNode(nodes_ids[0]).value();
    auto n2 = rand_g->getNode(nodes_ids[nodes_ids.size() - 1]).value();

    auto oracle = new alg::Oracle(g);
    auto result = oracle->query(n1, n2);
    if (result)
        std::cout << "eta from " << n1->getId() << " to " << n2->getId() << " is: " << result.value().second << std::endl;
    else
        std::cout << "no path found" << std::endl;

    std::cout << std::endl;

    auto linear_sep = new alg::LinearSeparator(rand_g);


    return 0;
}
