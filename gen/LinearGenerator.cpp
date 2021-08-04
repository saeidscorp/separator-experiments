//
// Created by saeid on 10/23/20.
//

#include "LinearGenerator.hpp"
#include "effolkronium/random.hpp"

using Random = effolkronium::random_static;

using namespace gen;

template<bool bidirectional_graph>
LinearGenerator<bidirectional_graph>::LinearGenerator(int length) {
    _length = length;
}

template<bool bidirectional_graph>
LinearGenerator<bidirectional_graph>::LinearGenerator() : _length(10) {}

template<bool bidirectional_graph>
model::Graph<bidirectional_graph> *LinearGenerator<bidirectional_graph>::generate() {
    auto graph = new model::Graph<bidirectional_graph>;

    model::Node *prev_node = nullptr;
    for (int i = 0; i < _length; ++i) {
        auto new_node = graph->createNode(i + 1, 0, "node_" + std::to_string(i + 1));
        if (prev_node != nullptr) {
            auto edge = graph->connect(prev_node, new_node);
            edge->setEta(Random::get(0., 1.));
        }
        prev_node = new_node;
    }

    return graph;
}
