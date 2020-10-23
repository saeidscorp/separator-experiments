//
// Created by saeid on 5/13/20.
//

#ifndef SEPARATOR_GRAPH_HPP
#define SEPARATOR_GRAPH_HPP

#include <map>
#include <vector>

#include "Node.hpp"
#include "Edge.hpp"

namespace model {

    class Graph {
    private:
        std::map<int, Node *> *_node_map;
        std::map<int, Edge *> *_edge_map;

        int _current_node_id;
        int _current_edge_id;

        int _next_node();

        int _next_edge();

    public:
        Graph();

        Node *createNode();

        Node *createNode(double lon, double lat, std::string name = std::string(""));

        Edge *connect(Node *from, Node *to);

        Edge *connect(int from_id, int to_id);

        Edge *connect(Node *from, Node *to, double max_speed, double eta = 0, std::string name = std::string());

        Edge *connect(int from_id, int to_id, double max_speed, double eta = 0, std::string name = std::string());

        std::string dotString();

    };

}

#endif //SEPARATOR_GRAPH_HPP
