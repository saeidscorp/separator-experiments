//
// Created by saeid on 5/13/20.
//

#ifndef SEPARATOR_GRAPH_HPP
#define SEPARATOR_GRAPH_HPP

#include <map>
#include <vector>
#include <functional>

#include "Node.hpp"
#include "Edge.hpp"
#include "modeltypes.hpp"

namespace model {

    template<bool bidirectional_graph = true>
    class Graph {
    private:
        std::map<int, Node *> *_node_map;
        std::map<int, Edge *> *_edge_map;

        int _current_node_id;
        int _current_edge_id;

        int _next_node_id();

        int _next_edge_id();

        void add_node(Node *node);

        template<bool bidirectional = bidirectional_graph>
        void add_edge(Edge *edge);

    public:
        Graph();

        Graph(const Graph& other);

        Node *createNode();

        Node *createNode(Node *node);

        Node *createNode(double lon, double lat, std::string name = std::string(""));

        template<bool bidirectional = bidirectional_graph>
        Edge *connect(Node *from, Node *to);

        template<bool bidirectional = bidirectional_graph>
        Edge *connect(Node *from, Node *to, Edge *edge);

        template<bool bidirectional = bidirectional_graph>
        Edge *connect(int from_id, int to_id);

        template<bool bidirectional = bidirectional_graph>
        Edge *connect(Node *from, Node *to, double max_speed, ETA eta = ETA{}, std::string name = std::string());

        template<bool bidirectional = bidirectional_graph>
        Edge *connect(int from_id, int to_id, double max_speed, ETA eta = ETA{}, std::string name = std::string());

        [[nodiscard]] std::vector<model::Node *> getNodes() const;

        [[nodiscard]] std::vector<int> getNodeIds() const;

        [[nodiscard]] std::vector<model::Edge *> getEdges() const;

        [[nodiscard]] std::vector<int> getEdgeIds() const;

        [[nodiscard]] std::optional<model::Node *> getNode(int id) const;

        [[nodiscard]] std::optional<model::Edge *> getEdge(int id) const;

        std::optional<model::Edge *> getEdgeBetween(model::Node *from, model::Node *to) const;

        [[nodiscard]] std::optional<model::Edge *> getEdgeBetween(int id1, int id2) const;

        [[nodiscard]] std::string dotString() const;

        double similarity(model::Graph<bidirectional_graph> *graph) const;

    };

}

#include "Graph.cpp"

#endif //SEPARATOR_GRAPH_HPP
