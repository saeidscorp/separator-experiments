//
// Created by saeid on 12/25/20.
//

#include "Tree.hpp"

using namespace model;

//template void Tree::build(const model::Graph<false> *);
//template void Tree::build(const model::Graph<true> *);

Tree::Tree() : Graph(), _root(nullptr) {}

template<bool bidirectional_graph>
Tree::Tree(const Graph<bidirectional_graph> *tree_graph, const Node *root) : _root(nullptr) {
    build(tree_graph, root);
}

template Tree::Tree(const Graph<true> *tree_graph, const Node *root);

template Tree::Tree(const Graph<false> *tree_graph, const Node *root);

Tree::Tree(const Tree &other) : Graph<false>(other),
                                _root(*getNode(other._root->getId())) {
    std::transform(other._parents.cbegin(), other._parents.cend(), std::inserter(_parents, _parents.end()),
                   [&](const auto &p) {
        return std::make_pair(*getNode(p.first->getId()), *getNode(p.second->getId()));
    });
}

template<bool bidirectional_graph>
void Tree::build(const model::Graph<bidirectional_graph> *graph, const model::Node *root) {

    if (graph->size() < 1)
        return;

    auto dfs = util::recursive_lambda([&](auto &&dfs, const model::Node *node) -> Node * {

        auto our_node = createNode(node);

        for (auto edge : node->getEdges()) {
            auto neighbor = edge->getOther(node);

            if constexpr (bidirectional_graph)
                if (neighbor == node) continue;

            auto our_neighbor = dfs(neighbor);

            connect(our_node, our_neighbor, edge);
            _parents[our_neighbor] = our_node;
        }

        return our_node;
    });

    if (root == nullptr)
        root = graph->getNodes()[0];

    _root = dfs(root);
}

Graph<false> *Tree::toGraph() const {
    return new Graph(*this);
}

Node *Tree::operator[](Node *node) const {
    return _parents.at(node);
}

Node *Tree::root() const {
    return _root;
}

std::string Tree::dotString() const {

    std::string edge_specifier = " -> ", graph_specifier = " digraph ";

    std::stringstream ss;
    ss << graph_specifier << " {" << std::endl;

    for (auto iter = this->_node_map->begin(); iter != this->_node_map->end(); iter++) {
        auto node = iter->second;
        std::string *name = node->getName();
        if (name->empty()) continue;

        ss << "    " << node->getId();
        ss << " [label=\"" << *name << "\"];" << std::endl;
    }

    for (auto iter = this->_edge_map->begin(); iter != this->_edge_map->end(); iter++) {
        auto edge = iter->second;
        ss << "    ";
        ss << edge->getFrom()->getId() << edge_specifier << edge->getTo()->getId();
        ss << std::endl;
    }

    ss << "}" << std::endl;
    return ss.str();
}
