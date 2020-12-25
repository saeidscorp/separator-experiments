//
// Created by saeid on 12/24/20.
//

#include "CentroidDecomposition.hpp"

using namespace alg;

template<bool bidirectional_graph>
CentroidDecomposition::CentroidDecomposition(const model::Graph<bidirectional_graph> *tree_graph) {
    build(tree_graph);
}

template CentroidDecomposition::CentroidDecomposition(const model::Graph<true> *tree_graph);

template CentroidDecomposition::CentroidDecomposition(const model::Graph<false> *tree_graph);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

template<bool bidirectional_graph>
void CentroidDecomposition::build(const Graph<bidirectional_graph> *graph) {

    auto tree_copy = model::Graph{*graph};

    std::map<model::Node *, std::size_t> subtree_sizes;
    std::map<model::Node *, model::Node *> original_parents;

    auto original_node = [&](model::Node *node) {
        auto our_id = node->getId();
        auto their_id = graph->getNode(our_id);
        return *their_id;
    };

    auto subtree_size = util::recursive_lambda([&](auto &&subtree_size,
                                                   model::Node *node,
                                                   model::Node *parent) -> std::size_t {
        subtree_sizes[node] = 1;

        for (auto neighbor : node->getNeighs())
            if (neighbor != parent)
                subtree_sizes[node] += subtree_size(neighbor, node);

        return subtree_sizes[node];
    });

    auto find_centroid = util::recursive_lambda([&](auto &&find_centroid,
                                                    model::Node *node,
                                                    model::Node *parent,
                                                    std::size_t n) -> model::Node * {

        for (auto neighbor : node->getNeighs())
            if (neighbor != parent and subtree_sizes[neighbor] > n / 2)
                return find_centroid(neighbor, node, n);

        return node;
    });

    auto make_tree = util::recursive_lambda([&](auto &&make_tree,
                                                model::Node *node,
                                                model::Node *parent) -> model::Node * {

        auto n = subtree_size(node, parent);
        auto centroid = find_centroid(node, parent, n);

        if (parent == nullptr)
            parent = centroid;

        auto original_centroid = original_node(centroid);
        original_parents[original_centroid] = original_node(parent);

        for (auto neighbor : centroid->getNeighs()) {
            tree_copy.disconnect(centroid, neighbor);
            make_tree(neighbor, centroid);
        }

        return original_centroid;
    });

    auto original_root = make_tree(tree_copy.getNodes()[0], nullptr);

    // create the tree structure from parent info

    std::map<model::Node *, model::Node *> matching;

    std::transform(original_parents.begin(), original_parents.end(), std::inserter(matching, matching.end()),
                   [&](const auto &p) {
                       return std::make_pair(p.first, createNode(p.first));
                   });

    auto connect_parent = [&](Node *node) {
        auto parent = original_parents.at(node);
        if (node == parent)
            return;
        auto our_node = matching.at(node), our_parent = matching.at(parent);
        connect(our_parent, our_node);
        _parents[our_node] = our_parent;
    };

    for (auto &node : util::extract_keys(original_parents))
        connect_parent(node);

    _root = matching.at(original_root);

}

#pragma clang diagnostic pop
