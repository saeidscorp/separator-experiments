//
// Created by saeid on 5/13/20.
//

#include "Node.hpp"

#include <utility>

using namespace model;

Node::Node(int id) : Node(id, 0, 0) {}

Node::Node(int id, double lon, double lat) : Node(id, lon, lat, std::string("")) {}

Node::Node(int id, double lon, double lat, std::string name) : _id(id), _lon(lon), _lat(lat),
                                                               _name(new std::string(std::move(name))) {
    _edges = new std::vector<Edge *>;
    _neighs = new std::map<int, Node *>;
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

const std::vector<Edge *> *Node::getEdges() const {
    return _edges;
}

std::map<int, Node *> *Node::getNeighs() const {
    return _neighs;
}

void Node::addNeigh(Node *other) {
    _neighs->insert({other->getId(), other});
}

void Node::addEdge(Edge *edge) {
    _edges->push_back(edge);
}

