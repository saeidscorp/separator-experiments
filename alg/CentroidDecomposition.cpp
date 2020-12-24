//
// Created by saeid on 12/24/20.
//

#include "CentroidDecomposition.h"

using namespace alg;

CentroidDecomposition::CentroidDecomposition(const model::Graph<true> *tree_graph) {
    build(tree_graph);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

void CentroidDecomposition::build(const Graph<true> *graph) {

    auto tree_copy = model::Graph<true>{*graph};

    auto original_node = [&](model::Node *node) {
        auto our_id = node->getId();
        auto their_id = graph->getNode(our_id);
        return *their_id;
    };

    auto subtree_size = util::recursive_lambda([&](auto &&subtree_size,
                                                   model::Node *node,
                                                   model::Node *parent) -> std::size_t {
        _subtree_size[node] = 1;

        for (auto neighbor : node->getNeighs())
            if (neighbor != parent)
                _subtree_size[node] += subtree_size(neighbor, node);

        return _subtree_size[node];
    });

    auto find_centroid = util::recursive_lambda([&](auto &&find_centroid,
                                                    model::Node *node,
                                                    model::Node *parent,
                                                    std::size_t n) -> model::Node * {

        for (auto neighbor : node->getNeighs())
            if (neighbor != parent and _subtree_size[neighbor] > n / 2)
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

        _parent[original_node(centroid)] = original_node(parent);

        for (auto neighbor : centroid->getNeighs()) {
            tree_copy.disconnect(centroid, neighbor);
            make_tree(neighbor, centroid);
        }

        return centroid;
    });

    root = make_tree(tree_copy.getNodes()[0], nullptr);

}

model::Graph<false> *CentroidDecomposition::toGraph() const {
    auto graph = new model::Graph<false>;

    std::map<model::Node *, bool> connected;

    std::transform(_parent.cbegin(), _parent.cend(), std::inserter(connected, connected.end()),
                   [&](const auto &p) {
                       return std::make_pair(p.first, false);
                   });

    auto connect_parent = [&](model::Node *node) {
        if (connected[node]) return;
        auto new_node = graph->createNode(node);
        connected[node] = true;
        auto parent = _parent.at(node);
        if (node == parent)
            return;
        graph->connect(parent, new_node);
    };

    for (auto &node : util::extract_keys(_parent))
        connect_parent(node);

    return graph;
}

#pragma clang diagnostic pop
