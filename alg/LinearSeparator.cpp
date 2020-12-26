//
// Created by saeid on 11/4/20.
//

#include "LinearSeparator.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <execution>

#include "effolkronium/random.hpp"
#include "../util/parallelutils.hpp"

using Random = effolkronium::random_static;

using namespace alg;

template<bool bidirectional_graph>
LinearSeparator<bidirectional_graph>::LinearSeparator(Oracle<bidirectional_graph> *oracle)
        : Separator<bidirectional_graph>(oracle) {
    preprocess(oracle);
}

template<bool bidirectional_graph>
LinearSeparator<bidirectional_graph>::LinearSeparator(const model::Graph<bidirectional_graph> *graph)
        : LinearSeparator<bidirectional_graph>(new Oracle(graph)) {}

template<bool bidirectional_graph>
void LinearSeparator<bidirectional_graph>::preprocess(Oracle<bidirectional_graph> *oracle) {
    auto starting_count = oracle->queries();
    auto all_nodes = oracle->getGraph()->getNodes();
    int start_index = 0, end_index = all_nodes.size() - 1;

    this->selected_nodes.push_back(all_nodes[start_index]);

    if (this->_seps_count > 2)
        std::sample(std::next(all_nodes.begin()), std::prev(all_nodes.end()),
                    std::back_inserter(this->selected_nodes),
                    this->_seps_count - 2,
                    Random::engine());

    this->selected_nodes.push_back(all_nodes[end_index]);

    auto sum_of_path_lengths = 0;

    std::transform(all_nodes.cbegin(), all_nodes.cend(), std::inserter(this->marked, this->marked.end()),
                   [&](const auto &p) {
                       return std::make_pair(p->getId(), false);
                   });

    Node *prev = nullptr;
    for (auto i = 0ul; i < this->_seps_count; ++i) {
        auto current = this->selected_nodes[i];
        this->marked[current->getId()] = true;
        if (prev) {

            auto place_in_table = [this, &sum_of_path_lengths](const auto &node_pair, const auto &result) {
                if (!result) return;
                auto result_path_len = result.value().first.size();
                auto result_eta = result.value().second;

                sum_of_path_lengths += result_path_len;
                this->table.emplace(node_pair, result_eta);
            };

            endpoints node_pair{prev, current};
            auto result = oracle->query(node_pair);
            place_in_table(node_pair, result);

            decltype(result) result_rev;

            if constexpr (!bidirectional_graph) {
                endpoints node_pair_rev{current, prev};
                result_rev = oracle->query(node_pair_rev);
                place_in_table(node_pair_rev, result);
            }

            if (!(result || result_rev))
                std::cerr << "there is a disconnection between the linear graph's nodes." << std::endl;

        }
        prev = current;
    }

    this->_avg_path_length = (ETA) sum_of_path_lengths / (this->_seps_count - 1);
    this->preprocessing_num_queries = oracle->queries() - starting_count;
}

template<bool bidirectional_graph>
model::ETA
LinearSeparator<bidirectional_graph>::eta_selectives(nodes_iterator from_it,
                                                     nodes_iterator to_it) const {

    if (from_it == this->selected_nodes.end() || to_it == this->selected_nodes.end())
        throw std::runtime_error("Inputs to the `eta_selectives` must be part of selectives.");

    auto accumulate = [this](auto begin, auto end) {
        return std::accumulate(begin, end, 0.,
                               [this](const auto &sum, const auto &pair) {
                                   const auto &[left, right] = pair;
                                   auto lookup = this->table.find({left, right});
                                   if (sum < 0 || lookup == this->table.end())
                                       return -1.;
                                   return sum + (*lookup).second;
                               });
    };

    if (from_it < to_it) {
        auto adj_selected = util::adjacent_pairs(from_it, std::next(to_it));
        return accumulate(adj_selected.begin(), adj_selected.end());
    } else {
        auto adj_selected = util::adjacent_pairs<true>(to_it, std::next(from_it));
        return accumulate(adj_selected.begin(), adj_selected.end());  // fixme: get rid of reverse
    }

}
