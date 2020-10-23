//
// Created by saeid on 5/13/20.
//

#include "Graph.hpp"
#include <sstream>

using namespace model;

Graph::Graph() {
    _node_map = new std::map<int, Node *>;
    _edge_map = new std::map<int, Edge *>;
    _current_node_id = 0;
    _current_edge_id = 0;
}

Node *Graph::createNode() {
    auto node = new Node(_next_node());
    _node_map->insert({node->getId(), node});
    return node;
}

Node *Graph::createNode(double lon, double lat, std::string name) {
    auto node = createNode();
    node->setLon(lon);
    node->setLat(lat);
    node->setName(name);
    return node;
}

int Graph::_next_node() {
    return ++_current_node_id;
}

Edge *Graph::connect(Node *from, Node *to) {
    auto edge = new Edge(from, to, _next_edge());
    _edge_map->insert({edge->getId(), edge});
    return edge;
}

int Graph::_next_edge() {
    return ++_current_edge_id;
}

Edge *Graph::connect(Node *from, Node *to, double max_speed, double eta, std::string name) {
    auto edge = connect(from, to);
    edge->setMaxSpeed(max_speed);
    edge->setEta(eta);
    edge->setName(new std::string(std::move(name)));
    return edge;
}

Edge *Graph::connect(int from_id, int to_id) {
    auto from_node = _node_map->at(from_id);
    auto to_node = _node_map->at(to_id);
    return connect(from_node, to_node);
}

Edge *Graph::connect(int from_id, int to_id, double max_speed, double eta, std::string name) {
    auto from_node = _node_map->at(from_id);
    auto to_node = _node_map->at(to_id);
    return connect(from_node, to_node, max_speed, eta, name);
}

std::string Graph::dotString() {
    std::stringstream ss;
    ss << "graph {" << std::endl;
    ss << "    " << "rankdir=\"LR\";" << std::endl;

    for (auto iter = _node_map->begin(); iter != _node_map->end(); iter++) {
        auto node = iter->second;
        std::string *name = node->getName();
        if (name->empty()) continue;

        ss << "    " << node->getId();
        ss << " [label=\"" << name << "\"];" << std::endl;
    }

    for (auto iter = _edge_map->begin(); iter != _edge_map->end(); iter++) {
        auto edge = iter->second;
        ss << "    ";
        ss << edge->getFrom()->getId() << " -- " << edge->getTo()->getId();
        ss << " [label=\"" << edge->getEta() << "\"];" << std::endl;
    }

    ss << "}" << std::endl;
    return ss.str();
}
