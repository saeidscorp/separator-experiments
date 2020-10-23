//
// Created by saeid on 10/23/20.
//

#include "LinearGenerator.hpp"
#include "effolkronium/random.hpp"

using Random = effolkronium::random_static;

using namespace gen;

LinearGenerator::LinearGenerator(int length) {
    _length = length;
}

LinearGenerator::LinearGenerator() : _length(10) {}

model::Graph *LinearGenerator::generate() {
    auto graph = new model::Graph;

    model::Node *prev_node = nullptr;
    for (int i = 0; i < _length; ++i) {
        auto new_node = graph->createNode();
        if (prev_node != nullptr) {
            auto edge = graph->connect(prev_node, new_node);
            edge->setEta(Random::get(0., 1.));
        }
        prev_node = new_node;
    }
    return graph;
}
