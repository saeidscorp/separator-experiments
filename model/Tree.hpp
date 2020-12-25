//
// Created by saeid on 12/25/20.
//

#ifndef SEPARATOR_TREE_HPP
#define SEPARATOR_TREE_HPP

#include "Graph.hpp"

namespace model {

    class Tree : public Graph<false> {
    protected:

        std::map<model::Node *, model::Node *> _parents;

        model::Node *_root;

        template<bool bidirectional_graph>
        void build(const model::Graph<bidirectional_graph> *graph, const model::Node *root);

    public:

        Tree();

        template<bool bidirectional_graph>
        explicit Tree(const Graph<bidirectional_graph> *tree_graph, const Node *root = nullptr);

        Tree(const Tree &other);

        Node *operator[](Node *node) const;

        [[nodiscard]] model::Graph<false> *toGraph() const;

    };

}

#endif //SEPARATOR_TREE_HPP
