//
// Created by saeid on 5/13/20.
//

#include "Graph.hpp"
#include <sstream>
#include <optional>

#include "../util/stlutils.hpp"

using namespace model;

template<bool bidirectional_graph>
Graph<bidirectional_graph>::Graph() {
    _node_map = new std::map<int, Node *>;
    _edge_map = new std::map<int, Edge *>;
    _current_node_id = 0;
    _current_edge_id = 0;
}

template<bool bidirectional_graph>
Node *Graph<bidirectional_graph>::createNode() {
    auto node = new Node(_next_node());
    _node_map->insert({node->getId(), node});
    return node;
}

template<bool bidirectional_graph>
Node *Graph<bidirectional_graph>::createNode(double lon, double lat, std::string name) {
    auto node = createNode();
    node->setLon(lon);
    node->setLat(lat);
    node->setName(name);
    return node;
}

template<bool bidirectional_graph>
int Graph<bidirectional_graph>::_next_node() {
    return ++_current_node_id;
}

template<bool bidirectional_graph>
int Graph<bidirectional_graph>::_next_edge() {
    return ++_current_edge_id;
}

template<bool bidirectional_graph>
template<bool bidirectional>
Edge *Graph<bidirectional_graph>::connect(Node *from, Node *to) {
    auto edge = from->connect(to);
    if constexpr (bidirectional) {
        to->connect(edge);
    }
    edge->setId(_next_edge());
    _edge_map->insert({edge->getId(), edge});
    return edge;
}

template<bool bidirectional_graph>
template<bool bidirectional>
Edge *Graph<bidirectional_graph>::connect(Node *from, Node *to, double max_speed, double eta, std::string name) {
    auto edge = connect<bidirectional>(from, to);
    edge->setMaxSpeed(max_speed);
    edge->setEta(eta);
    edge->setName(new std::string(std::move(name)));
    return edge;
}

template<bool bidirectional_graph>
template<bool bidirectional>
Edge *Graph<bidirectional_graph>::connect(int from_id, int to_id) {
    auto from_node = _node_map->at(from_id);
    auto to_node = _node_map->at(to_id);
    return connect<bidirectional>(from_node, to_node);
}

template<bool bidirectional_graph>
template<bool bidirectional>
Edge *Graph<bidirectional_graph>::connect(int from_id, int to_id, double max_speed, double eta, std::string name) {
    auto from_node = _node_map->at(from_id);
    auto to_node = _node_map->at(to_id);
    return connect<bidirectional>(from_node, to_node, max_speed, eta, name);
}

template<bool bidirectional_graph>
std::string Graph<bidirectional_graph>::dotString() {
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

template<bool bidirectional_graph>
std::vector<model::Node *> Graph<bidirectional_graph>::getNodes() {
    return util::extract_values(*_node_map);
}

template<bool bidirectional_graph>
std::vector<int> Graph<bidirectional_graph>::getNodeIds() {
    return util::extract_keys(*_node_map);
}

template<bool bidirectional_graph>
std::vector<model::Edge *> Graph<bidirectional_graph>::getEdges() {
    return util::extract_values(*_edge_map);
}

template<bool bidirectional_graph>
std::vector<int> Graph<bidirectional_graph>::getEdgeIds() {
    return util::extract_keys(*_edge_map);
}

template<bool bidirectional_graph>
std::optional<model::Node *> Graph<bidirectional_graph>::getNode(int id) {
    auto it = _node_map->find(id);
    if (it != _node_map->end())
        return {it->second};
    return {};
}

//namespace model {
//    template class Graph<true>;
//    template class Graph<false>;
//}
