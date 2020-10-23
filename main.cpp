
#include <iostream>

#include "model/model.hpp"

int main() {
    std::cout << "Simple benchmark for randomized node selection" << std::endl;

    auto g = new model::Graph;

    auto n1 = g->createNode(10, 20, std::string("node_1"));
    auto n2 = g->createNode(0, -10, std::string("node_2"));

    g->connect(n1, n2, 100, 0, std::string("edge_1"));

    return 0;
}
