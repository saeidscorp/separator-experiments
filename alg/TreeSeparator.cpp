//
// Created by saeid on 12/25/20.
//

#include "TreeSeparator.hpp"

#include <queue>
#include <map>

#include "CentroidDecomposition.hpp"
#include "../util/funcutils.hpp"

using namespace alg;

template<bool bidirectional_graph>
TreeSeparator<bidirectional_graph>::TreeSeparator(Oracle<bidirectional_graph> *oracle)
        : Separator<bidirectional_graph>(oracle) {
    preprocess(oracle);
}

template<bool bidirectional_graph>
TreeSeparator<bidirectional_graph>::TreeSeparator(const model::Graph<bidirectional_graph> *graph)
        : TreeSeparator<bidirectional_graph>(new Oracle(graph)) {}

template<bool bidirectional_graph>
void TreeSeparator<bidirectional_graph>::preprocess(Oracle<bidirectional_graph> *oracle) {
    auto starting_count = oracle->queries();
    auto all_nodes = oracle->getGraph()->getNodes();

    CentroidDecomposition cd{oracle->getGraph()};

    auto bfs = [&](model::Node *root, int max_size) {

        if (root == nullptr)
            return;

        std::map<model::Node *, bool> visited;
        std::queue<model::Node *> queue{{root}};

        visited[root] = true;

        int current_size = 0;
        while (!queue.empty()) {

            if (current_size >= max_size)
                break;

            auto node = queue.front();
            queue.pop();
            this->selected_nodes.push_back(node);  // FIXME: make sure what needs to be put in selected_nodes
            this->marked[node->getId()] = true;
            current_size++;

            for (auto neighbor : node->getNeighs())
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    queue.push(neighbor);
                }
        }
    };

    bfs(cd.root(), this->_seps_count);

    int selected_count = this->selected_nodes.size();
    std::transform(all_nodes.cbegin(), all_nodes.cend(), std::inserter(this->marked, this->marked.end()),
                   [&](const auto &p) {
                       return std::make_pair(p->getId(), false);
                   });

    auto dfs = util::recursive_lambda([&](auto &&dfs, model::Node *node, model::Node *parent) -> bool {
        auto marked_subtrees_count = 0;
        for (auto neighbor : node->getNeighs()) {
            if (neighbor == parent) continue;
            if (dfs(neighbor, node))
                marked_subtrees_count++;
        }
        if (marked_subtrees_count > 1) {
            this->selected_nodes.push_back(node);
            this->marked[node->getId()] = true;
        }
        return marked_subtrees_count || this->marked[node->getId()];
    });

    auto original_root = oracle->getGraph()->getNodes()[0];
    dfs(original_root, original_root);

    auto new_size = this->selected_nodes.size();
    std::cout << "selected nodes went from " << selected_count << " to " << new_size
              << " (increased by " << new_size - selected_count << ")" << std::endl;

    auto sum_of_path_lengths = 0;

//    Node *prev = nullptr;
//    for (auto i = 0ul; i < this->_seps_count; ++i) {
//        auto current = this->selected_nodes[i];
//        if (prev) {
//
//            auto place_in_table = [this, &sum_of_path_lengths](const auto &node_pair, const auto &result) {
//                if (!result) return;
//                auto result_path_len = result.value().first.size();
//                auto result_eta = result.value().second;
//
//                sum_of_path_lengths += result_path_len;
//                this->table.emplace(node_pair, result_eta);
//            };
//
//            endpoints node_pair{prev, current};
//            auto result = oracle->query(node_pair);
//            place_in_table(node_pair, result);
//
//            decltype(result) result_rev;
//
//            if constexpr (!bidirectional_graph) {
//                endpoints node_pair_rev{current, prev};
//                result_rev = oracle->query(node_pair_rev);
//                place_in_table(node_pair_rev, result);
//            }
//
//            if (!(result || result_rev))
//                std::cerr << "there is a disconnection between the linear graph's nodes." << std::endl;
//
//        }
//        prev = current;
//    }

    this->_avg_path_length = (ETA) sum_of_path_lengths / (this->_seps_count - 1);
    this->preprocessing_num_queries = oracle->queries() - starting_count;
}

template<bool bidirectional_graph>
ETA TreeSeparator<bidirectional_graph>::eta_selectives(nodes_iterator from_it,
                                                       nodes_iterator to_it) const {
    return 0;
}
