//
// Created by saeid on 5/13/20.
//

#include "Node.hpp"

#include <utility>
#include <cmath>

#include "../util/stlutils.hpp"

using namespace model;

Node::Node(int id) : Node(id, 0, 0) {}

Node::Node(int id, double lon, double lat) : Node(id, lon, lat, std::string()) {}

Node::Node(int id, double lon, double lat, std::string name) : _id(id), _lon(lon), _lat(lat),
                                                               _name(new std::string(std::move(name))) {
    _edges = new std::vector<Edge *>;
    _neighs = new std::map<int, Node *>;
    _edge_map = new std::map<Node *, Edge *>;
}

int Node::getId() const {
    return _id;
}

void Node::setId(int id) {
    _id = id;
}

double Node::getLon() const {
    return _lon;
}

void Node::setLon(double lon) {
    _lon = lon;
}

double Node::getLat() const {
    return _lat;
}

void Node::setLat(double lat) {
    _lat = lat;
}

std::string *Node::getName() const {
    return _name;
}

void Node::setName(std::string name) {
    _name = new std::string(std::move(name));
}

std::vector<Edge *> Node::getEdges() const {
    return *_edges;
}

std::vector<Node *> Node::getNeighs() const {
    return util::extract_values(*_neighs);
}

void Node::addNeigh(Node *other) {
    _neighs->insert({other->getId(), other});
}

std::optional<Edge *> Node::getEdgeOf(Node *node) {
    auto it = _edge_map->find(node);
    if (it != _edge_map->end())
        return it->second;
    return {};
}

double Node::distance(const Node *one, const Node *another) {
    auto dx = one->getLon() - another->getLon();
    auto dy = one->getLat() - another->getLat();
    return std::sqrt(std::pow(dx, 2) + std::pow(dy, 2));
}

Node::Node(const Node &other) : _id(other._id), _lon(other._lon), _lat(other._lat),
                                _name(new std::string(*other._name)) {
    _edges = new std::vector<Edge *>;
    _neighs = new std::map<int, Node *>;
    _edge_map = new std::map<Node *, Edge *>;
}
