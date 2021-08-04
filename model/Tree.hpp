//
// Created by saeid on 12/25/20.
//

#ifndef SEPARATOR_TREE_HPP
#define SEPARATOR_TREE_HPP

#include "Graph.hpp"

namespace model {

    class Tree : public Graph<false> {
    protected:

        void add_edge(Edge *edge) override;

        void remove_edge(Edge *edge) override;

        std::map<model::Node *, model::Node *> _parents;

        model::Node *_root;

        template<bool bidirectional_graph>
        void build(const model::Graph<bidirectional_graph> *graph, const model::Node *root);

    public:

        Tree();

        template<bool bidirectional_graph>
        explicit Tree(const Graph<bidirectional_graph> *tree_graph, const Node *root = nullptr);

        Tree(const Tree &other);

        std::optional<Node *> operator[](Node *node) const;

        [[nodiscard]] Node *root() const;

        void root(Node *root_node);

        [[nodiscard]] Graph<false> *toGraph() const;

        [[nodiscard]] std::string dotString() const override;

    };

}

#endif //SEPARATOR_TREE_HPP
