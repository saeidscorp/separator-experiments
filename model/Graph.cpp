//
// Created by saeid on 5/13/20.
//

#include "Graph.hpp"
#include <sstream>
#include <optional>
#include <cmath>
#include <numeric>
#include <execution>

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
void Graph<bidirectional_graph>::add_node(Node *node) {
    _node_map->insert({node->getId(), node});
}

template<bool bidirectional_graph>
Node *Graph<bidirectional_graph>::createNode() {
    auto node = new Node(_next_node_id());
    add_node(node);
    return node;
}

template<bool bidirectional_graph>
Node *Graph<bidirectional_graph>::createNode(const Node *node) {
    auto copy = new Node(*node);
    _current_node_id = std::max(_current_node_id, node->getId());
    add_node(copy);
    return copy;
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
int Graph<bidirectional_graph>::_next_node_id() {
    return ++_current_node_id;
}

template<bool bidirectional_graph>
int Graph<bidirectional_graph>::_next_edge_id() {
    return ++_current_edge_id;
}

template<bool bidirectional_graph>
void Graph<bidirectional_graph>::add_edge(Edge *edge) {
    auto from = edge->getFrom();
    auto to = edge->getTo();
    from->connect(edge);
    if constexpr (bidirectional_graph) {
        to->connect(edge);
    }
    _edge_map->insert({edge->getId(), edge});
}

template<bool bidirectional_graph>
Edge *Graph<bidirectional_graph>::connect(Node *from, Node *to) {
    auto edge = from->connect(to);
    edge->setId(_next_edge_id());
    add_edge(edge);
    return edge;
}

template<bool bidirectional_graph>
Edge *Graph<bidirectional_graph>::connect(Node *from, Node *to, Edge *edge) {
    if (_edge_map->contains(edge->getId()))
        return edge;
    auto copy = new Edge(*edge);
    copy->setFrom(from);
    copy->setTo(to);
    add_edge(copy);
    return copy;
}

template<bool bidirectional_graph>
Edge *Graph<bidirectional_graph>::connect(Node *from, Node *to, double max_speed, ETA eta, std::string name) {
    auto edge = connect(from, to);
    edge->setMaxSpeed(max_speed);
    edge->setEta(eta);
    edge->setName(new std::string(std::move(name)));
    return edge;
}

template<bool bidirectional_graph>
Edge *Graph<bidirectional_graph>::connect(int from_id, int to_id) {
    auto from_node = _node_map->at(from_id);
    auto to_node = _node_map->at(to_id);
    return connect(from_node, to_node);
}

template<bool bidirectional_graph>
Edge *Graph<bidirectional_graph>::connect(int from_id, int to_id, double max_speed, ETA eta, std::string name) {
    auto from_node = _node_map->at(from_id);
    auto to_node = _node_map->at(to_id);
    return connect(from_node, to_node, max_speed, eta, name);
}

template<bool bidirectional_graph>
std::size_t Graph<bidirectional_graph>::size() const {
    return _node_map->size();
}

template<bool bidirectional_graph>
std::string Graph<bidirectional_graph>::dotString() const {

    std::string edge_specifier, graph_specifier;
    if constexpr (bidirectional_graph) {
        graph_specifier = "graph";
        edge_specifier = " -- ";
    } else {
        graph_specifier = "digraph";
        edge_specifier = " -> ";
    }

    std::stringstream ss;
    ss << graph_specifier << " {" << std::endl;
//    ss << "    " << "rankdir=\"LR\";" << std::endl;

    for (auto iter = _node_map->begin(); iter != _node_map->end(); iter++) {
        auto node = iter->second;
        std::string *name = node->getName();
        if (name->empty()) continue;

        ss << "    " << node->getId();
        ss << " [label=\"" << *name << "\"];" << std::endl;
    }

    for (auto iter = _edge_map->begin(); iter != _edge_map->end(); iter++) {
        auto edge = iter->second;
        ss << "    ";
        ss << edge->getFrom()->getId() << edge_specifier << edge->getTo()->getId() << std::endl;
//        ss << " [label=\"" << edge->getEta() << "\"];" << std::endl;
    }

    ss << "}" << std::endl;
    return ss.str();
}

template<bool bidirectional_graph>
std::vector<model::Node *> Graph<bidirectional_graph>::getNodes() const {
    return util::extract_values(*_node_map);
}

template<bool bidirectional_graph>
std::vector<int> Graph<bidirectional_graph>::getNodeIds() const {
    return util::extract_keys(*_node_map);
}

template<bool bidirectional_graph>
std::vector<model::Edge *> Graph<bidirectional_graph>::getEdges() const {
    return util::extract_values(*_edge_map);
}

template<bool bidirectional_graph>
std::vector<int> Graph<bidirectional_graph>::getEdgeIds() const {
    return util::extract_keys(*_edge_map);
}

template<bool bidirectional_graph>
std::optional<model::Node *> Graph<bidirectional_graph>::getNode(int id) const {
    auto it = _node_map->find(id);
    if (it != _node_map->end())
        return {it->second};
    return {};
}

template<bool bidirectional_graph>
std::optional<model::Edge *> Graph<bidirectional_graph>::getEdge(int id) const {
    auto it = _edge_map->find(id);
    if (it != _edge_map->end())
        return {it->second};
    return {};
}

template<bool bidirectional_graph>
Graph<bidirectional_graph>::Graph(const Graph<bidirectional_graph> &other)
        : _current_node_id(other._current_node_id),
          _current_edge_id(other._current_edge_id){

    _node_map = new std::map<int, Node *>;
    _edge_map = new std::map<int, Edge *>;

    std::map<Node *, Node *> visited_nodes;

    auto dfs = util::recursive_lambda([&visited_nodes, this](auto &&dfs, Node *node) -> Node * {
        if (visited_nodes.contains(node))
            return visited_nodes[node];

        auto our_node = createNode(node);
        visited_nodes.emplace(node, our_node);

        for (auto edge : node->getEdges()) {
            auto neigh = edge->getOther(node);
            auto our_neigh = dfs(neigh);
            connect(our_node, our_neigh, edge);
        }

        return our_node;
    });

    for (auto node : other.getNodes())
        dfs(node);
}

template<bool bidirectional_graph>
std::optional<model::Edge *> Graph<bidirectional_graph>::getEdgeBetween(model::Node *from, model::Node *to) const {
    auto edge_res = from->getEdgeOf(to);
    if (edge_res) return edge_res.value();
    if constexpr (bidirectional_graph) {
        edge_res = to->getEdgeOf(from);
        if (edge_res) return edge_res.value();
    }
    return {};
}

template<bool bidirectional_graph>
std::optional<model::Edge *> Graph<bidirectional_graph>::getEdgeBetween(int id1, int id2) const {
    auto n1 = getNode(id1);
    if (!n1) return {};
    auto n2 = getNode(id2);
    if (!n2) return {};
    return getEdgeBetween(n1.value(), n2.value());
}

template<bool bidirectional_graph>
double Graph<bidirectional_graph>::similarity(model::Graph<bidirectional_graph> *graph) const {
    auto other_graph = graph;

    auto edge_ids = other_graph->getEdgeIds();
    auto begin = edge_ids.cbegin(), end = edge_ids.cend();

    std::vector<std::pair<double, int>> results;
    results.reserve(std::distance(begin, end));

    std::transform(begin, end, std::back_inserter(results),
                   [&](const auto &other_id) {
                       auto other_edge_o = other_graph->getEdge(other_id);
                       auto our_edge_o = this->getEdge(other_id);

                       if (!other_edge_o || !our_edge_o)
                           throw std::runtime_error("Topologies or edge ids mismatch.");

                       auto other_edge = other_edge_o.value();
                       auto our_edge = our_edge_o.value();

                       auto sse = std::pow(other_edge->getEta() - our_edge->getEta(), 2);
                       return std::make_pair(sse, 1);
                   });

    const auto &[sse, edge_count] =
    std::reduce(std::execution::par_unseq, results.cbegin(), results.cend(),
                std::make_pair(0., 0),
                [](const auto &p1, const auto &p2) {
                    return std::make_pair(p1.first + p2.first, p1.second + p2.second);
                });

    return sse / edge_count;
}

template<bool bidirectional_graph>
void Graph<bidirectional_graph>::remove_edge(Edge *edge) {
    auto from = edge->getFrom();
    auto to = edge->getTo();
    from->disconnect(edge);
    if constexpr (bidirectional_graph) {
        to->disconnect(edge);
    }
    _edge_map->erase(edge->getId());
}

template<bool bidirectional_graph>
void Graph<bidirectional_graph>::disconnect(Node *from, Node *to) {
    auto edge = from->getEdgeOf(to);
    if (edge)
        remove_edge(*edge);
}

template<bool bidirectional_graph>
void Graph<bidirectional_graph>::disconnect(int from_id, int to_id) {
    auto from = _node_map->at(from_id), to = _node_map->at(to_id);
    disconnect(from, to);
}

template<bool bidirectional_graph>
void Graph<bidirectional_graph>::disconnect(Edge *edge) {
    remove_edge(edge);
}

namespace model {
    template class Graph<true>;
    template class Graph<false>;
}
