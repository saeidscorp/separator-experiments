//
// Created by saeid on 11/4/20.
//

#include "LinearSeparator.h"

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
        : Oracle<bidirectional_graph>(oracle->getGraph()), preprocessing_num_queries(0) {


    auto nodes_count = oracle->getGraph()->getNodes().size();
    _seps_count = std::sqrt(std::max(nodes_count, 2ul) - 2) + 2;

    preprocess(oracle);
}

template<bool bidirectional>
LinearSeparator<bidirectional>::LinearSeparator(model::Graph<bidirectional> *graph)
        : LinearSeparator(new Oracle(graph)) {}

template<bool bidirectional_graph>
void LinearSeparator<bidirectional_graph>::preprocess(Oracle<bidirectional_graph> *oracle) {
    auto starting_count = oracle->queries();
    auto all_nodes = oracle->getGraph()->getNodes();
    int start_index = 0, end_index = all_nodes.size() - 1;

    selected_nodes.push_back(all_nodes[start_index]);

    if (_seps_count > 2)
        std::sample(std::next(all_nodes.begin()), std::prev(all_nodes.end()),
                    std::back_inserter(selected_nodes),
                    _seps_count - 2,
                    Random::engine());

    selected_nodes.push_back(all_nodes[end_index]);

    auto sum_of_path_lengths = 0;

    Node *prev = nullptr;
    for (auto i = 0ul; i < _seps_count; ++i) {
        auto current = selected_nodes[i];
        if (prev) {

            auto place_in_table = [this, &sum_of_path_lengths](const auto &node_pair, const auto &result) {
                if (!result) return;
                auto result_path_len = result.value().first.size();
                auto result_eta = result.value().second;

                sum_of_path_lengths += result_path_len;
                table.emplace(node_pair, result_eta);
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

    _avg_path_length = (ETA) sum_of_path_lengths / (_seps_count - 1);
    preprocessing_num_queries = oracle->queries() - starting_count;
}

template<bool bidirectional_graph>
model::ETA LinearSeparator<bidirectional_graph>::eta_selectives(decltype(selected_nodes)::const_iterator from_it,
                                                                decltype(selected_nodes)::const_iterator to_it) const {
    if (from_it == selected_nodes.end() || to_it == selected_nodes.end())
        throw std::runtime_error("Inputs to the `eta_selectives` must be part of selectives.");

    auto accumulate = [this](auto begin, auto end) {
        return std::accumulate(begin, end, 0.,
                               [this](const auto &sum, const auto &pair) {
                                   const auto &[left, right] = pair;
                                   auto lookup = table.find({left, right});
                                   if (sum < 0 || lookup == table.end())
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

template<bool bidirectional_graph>
query_result LinearSeparator<bidirectional_graph>::do_query(model::endpoints ep) const {
    auto from = ep.first, to = ep.second;
    auto sel_from = closest_separator(from), sel_to = closest_separator(to);

    auto sel_eta = eta_selectives(sel_from, sel_to);

    // heuristic 1:
    auto sel2_from = closest_separator<2>(from), sel2_to = closest_separator<2>(to);

    // fixme: this breaks with unidirectional edges.
    auto eta_from_segment = eta_selectives(sel2_from, sel_from);
    auto eta_to_segment = eta_selectives(sel2_to, sel_to);
    auto ratio_from = model::Node::distance(from, *sel_from)
            / std::max(model::Node::distance(*sel2_from, *sel_from), 1.);
    auto ratio_to = model::Node::distance(to, *sel_to)
            / std::max(model::Node::distance(*sel2_to, *sel_to), 1.);

    auto heta_from = ratio_from * eta_from_segment, heta_to = ratio_to * eta_to_segment;
    auto total_eta = heta_from + sel_eta + heta_to;

    std::list<model::Node *> path;
    if (from != *sel_from) path.push_back(from);
    if (sel_from < sel_to)
        std::copy(sel_from, std::next(sel_to), std::back_inserter(path));
    else
        std::copy(std::reverse_iterator(std::next(sel_from)), std::reverse_iterator(sel_to),
                  std::back_inserter(path));
    if (to != *sel_to) path.push_back(to);

    return model::path_length(path, total_eta);
}

template<bool bidirectional_graph>
double LinearSeparator<bidirectional_graph>::similarity() const {
    Oracle<bidirectional_graph> oracle(this->getGraph());
    return oracle.similarity(this);
}

template<bool bidirectional_graph>
unsigned LinearSeparator<bidirectional_graph>::preprocessing_queries() const {
    return preprocessing_num_queries;
}
