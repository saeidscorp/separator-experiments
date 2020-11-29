//
// Created by saeid on 11/4/20.
//

#include "LinearSeparator.h"

#include <algorithm>
#include <cmath>

#include "effolkronium/random.hpp"

using Random = effolkronium::random_static;

using namespace alg;

template<bool bidirectional_graph>
LinearSeparator<bidirectional_graph>::LinearSeparator(Oracle<bidirectional_graph> *oracle)
        : oracle(oracle), graph(oracle->getGraph()) {

    auto nodes_count = graph->getNodes().size();
    _seps_count = std::sqrt(nodes_count) + 2;

    preprocess();
}

template<bool bidirectional>
LinearSeparator<bidirectional>::LinearSeparator(model::Graph<bidirectional> *graph)
        : LinearSeparator(new Oracle(graph)) {}

template<bool bidirectional_graph>
void LinearSeparator<bidirectional_graph>::preprocess() {
    auto all_nodes = graph->getNodes();
    int start_index = 0, end_index = all_nodes.size() - 1;

    selected_nodes.push_back(all_nodes[start_index]);

    std::sample(std::next(all_nodes.begin()), std::prev(all_nodes.end()),
                std::back_inserter(selected_nodes),
                _seps_count - 2,
                Random::engine());

    selected_nodes.push_back(all_nodes[end_index]);

    auto sum_of_path_lengths = 0;

    Node *prev = nullptr;
    for (auto i = 0; i < _seps_count; ++i) {
        auto current = selected_nodes[i];
        if (prev) {
            endpoints node_pair{prev, current};
            auto result = oracle->query(node_pair);
            if (!result)
                throw std::runtime_error("there is a disconnection between the linear graph's nodes.");

            auto result_path_len = result.value().first.size();
            auto result_eta = result.value().second;

            sum_of_path_lengths += result_path_len;
            table.emplace(node_pair, result_eta);
        }
        prev = current;
    }

    _avg_path_length = sum_of_path_lengths / (_seps_count - 1);
}
