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

template<bool bidirectional_graph>
Graph<bidirectional_graph> *model::random_weights(Graph<bidirectional_graph> *g) {
    auto g_copy = new Graph<bidirectional_graph>(*g);
    auto nodes = g->getNodes();
    auto start_index = 0ull, end_index = nodes.size() - 1;
    for (auto i = start_index; i < end_index; ++i)
        for (auto j = start_index; j < end_index; ++j) {
            auto from = nodes[i], to = nodes[j];
            auto eta = Random::get(0., 1.);
            auto our_edge = g_copy->getEdgeBetween(from->getId(), to->getId());
            if (our_edge)
                our_edge.value()->setEta(eta);
        }
    return g_copy;
}

template Graph<true> *model::random_weights(Graph<true> *g);
