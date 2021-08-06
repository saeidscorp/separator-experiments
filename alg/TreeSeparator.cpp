//
// Created by saeid on 12/25/20.
//

#include "TreeSeparator.hpp"

#include <queue>
#include <map>

#include "CentroidDecomposition.hpp"
#include "../util/funcutils.hpp"
#include "../util/printutils.hpp"

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
    auto graph = oracle->getGraph();
    auto all_nodes = graph->getNodes();

    CentroidDecomposition cd{graph};

//    util::visualize_graph(&cd);

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
            this->selected_nodes.push_back(*graph->getNode(node->getId()));
            this->marked[node->getId()] = true;
            current_size++;

            for (auto neighbor : node->getNeighs())
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    queue.push(neighbor);
                }
        }
    };

    std::transform(all_nodes.cbegin(), all_nodes.cend(), std::inserter(this->marked, this->marked.end()),
                   [&](const auto &p) {
                       return std::make_pair(p->getId(), false);
                   });
    bfs(cd.root(), this->_seps_count);

    auto fill_nodes = util::recursive_lambda([&](auto &&fill_nodes,
                                                 model::Node *node,
                                                 model::Node *parent) -> model::Node * {

        std::vector<model::Node *> marked_children;

        for (auto neighbor : node->getNeighs()) {
            if (neighbor == parent) continue;
            auto subtree_node = fill_nodes(neighbor, node);
            if (subtree_node != nullptr)
                marked_children.push_back(subtree_node);
        }

        bool marked = this->marked[node->getId()];
        if (marked_children.size() > 1 && !marked) {
            this->selected_nodes.push_back(node);
            this->marked[node->getId()] = marked = true;
        }

        if (marked) {
            auto new_node = decomposition.createNode(node);
            for (auto subtree_child : marked_children)
                decomposition.connect(new_node, subtree_child);
            return new_node;
        }
        if (!marked_children.empty())
            return marked_children[0];
        return nullptr;
    });

    int selected_count = this->selected_nodes.size();

    auto original_root = all_nodes[0];
    auto new_root = fill_nodes(original_root, original_root);

    auto new_size = this->selected_nodes.size();

    decomposition.root(new_root);

    auto sum_of_path_lengths = 0;

    auto place_in_table = [this, &sum_of_path_lengths](const auto &node_pair, const auto &result) {
        if (!result) return;
        auto result_path_len = result.value().first.size();
        auto result_eta = result.value().second;

        sum_of_path_lengths += result_path_len;
        this->table.emplace(node_pair, result_eta);
    };

    auto dfs = util::recursive_lambda([&](auto &&dfs, model::Node *node,
                                          model::Node *parent) -> void {

        if (parent != node) {
            auto original_parent = *graph->getNode(parent->getId());
            auto original_node = *graph->getNode(node->getId());

            model::endpoints node_pair{original_parent, original_node};
            auto result = oracle->query(node_pair);
            place_in_table(node_pair, result);

            decltype(result) result_rev;

            if constexpr (!bidirectional_graph) {
                model::endpoints node_pair_rev{original_node, original_parent};
                result_rev = oracle->query(node_pair_rev);
                place_in_table(node_pair_rev, result_rev);
            }

            if (!(result || result_rev))
                std::cerr << "there is a disconnection between the tree's nodes." << std::endl;
        }

        for (auto neighbor : node->getNeighs())
            if (neighbor != parent)
                dfs(neighbor, node);
    });

    dfs(decomposition.root(), decomposition.root());

    this->preprocessing_num_queries = oracle->queries() - starting_count;
    this->_avg_path_length = (model::ETA) sum_of_path_lengths / this->preprocessing_num_queries;

    std::cout << "selected nodes went from " << selected_count << " to " << new_size
              << " (increased by " << new_size - selected_count << ")" << std::endl;
}

template<bool bidirectional_graph>
model::path TreeSeparator<bidirectional_graph>::find_path(model::Node *from, model::Node *to) const {
    model::path path;

    auto path_to_root_in_decomp = [&](model::Node *node) {
        model::path root_path;

        auto parent = parent_in_decomposition(node);

        do {
            root_path.push_back(*this->graph->getNode(node->getId()));
            node = parent;
            parent = *decomposition[node];
        } while (parent);

        if (node)
            root_path.push_back(*this->graph->getNode(node->getId()));

        root_path.reverse();
        return root_path;
    };

    auto first_selected = util::recursive_lambda([&](auto &&first_selected,
            model::Node *node,
            model::Node *parent) -> model::Node * {

        if (this->marked.at(node->getId()))
            return node;

        for (auto neigh : node->getNeighs()) {
            if (neigh == parent) continue;
            auto first = first_selected(neigh, node);
            if (first)
                return first;
        }

        return nullptr;
    });

    auto sel_from  = first_selected(from, from), sel_to = first_selected(to, to);
    auto from_path = path_to_root_in_decomp(sel_from), to_path = path_to_root_in_decomp(sel_to);

    auto from_it = from_path.cbegin(), to_it = to_path.cbegin();
    for (; from_it != from_path.cend() && to_it != to_path.cend() &&
           *from_it == *to_it;
           ++from_it, ++to_it);

    std::copy(from_path.crbegin(), std::reverse_iterator(std::prev(from_it)), std::back_inserter(path));
    std::copy(to_it, to_path.cend(), std::back_inserter(path));

    return path;
}

template <bool bidirectional_graph>
model::Node * TreeSeparator<bidirectional_graph>::parent_in_decomposition(model::Node * node) const {
    auto node_id = node->getId();
    return *decomposition[*decomposition.getNode(node_id)];
}

namespace alg {
    template class TreeSeparator<true>;
    template class TreeSeparator<false>;
}
