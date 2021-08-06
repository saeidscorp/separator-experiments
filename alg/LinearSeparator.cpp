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

    model::Node *prev = nullptr;
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

            model::endpoints node_pair{prev, current};
            auto result = oracle->query(node_pair);
            place_in_table(node_pair, result);

            decltype(result) result_rev;

            if constexpr (!bidirectional_graph) {
                model::endpoints node_pair_rev{current, prev};
                result_rev = oracle->query(node_pair_rev);
                place_in_table(node_pair_rev, result);
            }

            if (!(result || result_rev))
                std::cerr << "there is a disconnection between the linear graph's nodes." << std::endl;

        }
        prev = current;
    }

    this->_avg_path_length = (model::ETA) sum_of_path_lengths / (this->_seps_count - 1);
    this->preprocessing_num_queries = oracle->queries() - starting_count;
}

template<bool bidirectional_graph>
model::path LinearSeparator<bidirectional_graph>::find_path(model::Node *from, model::Node *to) const {

    model::path path;

    auto sel_from = this->closest_separator(from, 1), sel_to = this->closest_separator(to, 1);
    auto sel2_from = *this->closest_separator(from, 2), sel2_to = *this->closest_separator(to, 2);

    if (sel_from < sel_to)
        std::copy(sel_from, std::next(sel_to), std::back_inserter(path));
    else
        std::copy(std::reverse_iterator(std::next(sel_from)), std::reverse_iterator(sel_to),
                  std::back_inserter(path));

    auto do_alt_from = true, do_alt_to = true;
    auto alt_from = false, alt_to = false;
    for (auto it = path.begin(); it != path.end(); ++it) {
        if (it == path.begin()) {
            if (*it == from)
                do_alt_from = false;
            continue;
        }
        if (std::next(it) == path.end()) {
            if (*it == to)
                do_alt_to = false;
            continue;
        }

        if (do_alt_from && (*it)->getId() == sel2_from->getId()) alt_from = true;
        if ((*it)->getId() == sel2_to->getId()) alt_to = true;
    }

    if (alt_from)
        path.pop_front();
    if (do_alt_to && alt_to)
        path.pop_back();

    return path;
}

template <bool bidirectional_graph>
selected_iterator LinearSeparator<bidirectional_graph>::closest_separator(model::Node * node, int nth) const {

    if (nth == 1)
        return util::min_by<1>(this->selected_nodes, [node](const model::Node * n) {
            return model::Node::distance(node, n);
        });
    else if (nth == 2)
        return util::min_by<2>(this->selected_nodes, [node](const model::Node * n) {
            return model::Node::distance(node, n);
        });
    else
        return util::min_by<3>(this->selected_nodes, [node](const model::Node * n) {
            return model::Node::distance(node, n);
        });
}

namespace alg {
    template class LinearSeparator<true>;
    template class LinearSeparator<false>;
}
