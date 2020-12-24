//
// Created by saeid on 5/13/20.
//

#include "model.hpp"
#include "Node.hpp"


using namespace model;

void Node::addEdge(Edge *edge) {
    auto other = edge->getOther(this);

    // fixme: assert that the values are also the same (is that even necessary?)
    if (_edge_map->contains(other))
        return;

    _edges->push_back(edge);
    addNeigh(other);
    _edge_map->insert({other, edge});
}

Edge *Node::connect(Node *other) {
    Edge *edge = new Edge(this, other);
    addEdge(edge);
    return edge;
}

Node *Node::connect(Edge *existingEdge) {
    addEdge(existingEdge);
    return existingEdge->getOther(this);
}

void Edge::connectFromTo() {
    _from->addNeigh(_to);
    _to->addNeigh(_from);

    _from->addEdge(this);
    _to->addEdge(this);
}

std::ostream &model::operator<< (std::ostream &o, Node const &node) {
    decltype(o) o2 = !node.getName()->empty() ? o << node.getName() : (o << "#" << node.getId());
    return o2 << "(" << node.getLon() << ", " << node.getLat() << ")";
}

std::ostream &model::operator<< (std::ostream &o, Node const *node) {
    return model::operator<<(o, *node);
}

std::ostream &model::operator<< (std::ostream &o, std::string const *string) {
    return o << *string;
}

void Node::remove_edge(Edge *edge) {
    auto other = edge->getOther(this);

    // fixme: assert that the values are also the same (is that even necessary?)
    if (!_edge_map->contains(other))
        return;

    _edges->erase(std::remove(_edges->begin(), _edges->end(), edge), _edges->end());
    remove_neigh(other);
    _edge_map->erase(other);
}

void Node::disconnect(Edge *edge) {
    remove_edge(edge);
}

void Node::disconnect(Node *other) {
    auto edge_it = _edge_map->find(other);
    if (edge_it != _edge_map->end())
        remove_edge(edge_it->second);
}
