//
// Created by saeid on 11/4/20.
//

#include "Oracle.hpp"

#include <map>
#include <queue>
#include <set>
#include <cmath>

#include "../util/stlutils.hpp"

using namespace alg;

template<bool bidirectional_graph>
std::optional<model::path_length> Oracle<bidirectional_graph>::shortest_path(model::endpoints ep) {

    model::Node *start = ep.first, *goal = ep.second;

    auto heuristic = [&](model::Node *node) {
        return model::Node::distance(node, goal) / model::max_speed;
    };


    typedef std::map<model::Node *, double> score_map;

    score_map g_score;
    score_map f_score;

    g_score[start] = 0;
    g_score[start] = heuristic(start);


    std::set<model::Node *> open_set;
    std::priority_queue open_pq{[&f_score](const auto &a, const auto &b) { return f_score[a] > f_score[b]; },
                                std::deque<model::Node *>{}};
    open_pq.push(start);


    std::map<model::Node *, model::Edge *> came_from;

    auto reconstruct_path = [&came_from](model::Node *current) {
        model::path total_path{current};
        double total_eta = 0;

        while (came_from.contains(current)) {
            auto prev_edge = came_from[current];
            current = prev_edge->getOther(current);
            total_path.push_front(current);
            total_eta += prev_edge->getEta();
        }

        return model::path_length(total_path, total_eta);
    };


    auto get_score = [](score_map *smap, model::Node *node) {
        return smap->contains(node) ? smap->at(node) : std::numeric_limits<double>::max();
    };


    while (!open_pq.empty()) {
        auto current = open_pq.top();

        if (current == goal)
            return reconstruct_path(current);

        open_pq.pop();

        for (auto neighbor : current->getNeighs()) {
            auto neighbor_edge = current->getEdgeOf(neighbor);
            auto tentative_g_score = get_score(&g_score, current) + neighbor_edge->getEta();

            if (tentative_g_score < get_score(&g_score, neighbor)) {
                came_from[neighbor] = neighbor_edge;
                g_score[neighbor] = tentative_g_score;
                f_score[neighbor] = g_score[neighbor] + heuristic(neighbor);

                if (open_set.contains(neighbor)) continue;

                open_set.insert(neighbor);
                open_pq.push(neighbor);
            }
        }
    }

    return {};
}

template<bool bidirectional_graph>
Oracle<bidirectional_graph>::Oracle(model::Graph<bidirectional_graph> *graph) : num_queries(0), graph(graph) {}

template<bool bidirectional_graph>
query_result Oracle<bidirectional_graph>::query(model::endpoints ep) {
    num_queries++;
    util::map_value_default(frequencies, ep, [] (int c) { return c + 1; });
    // todo: make this a part of Graph class and make it more robust
    if (graph->getNode(ep.first->getId()) != ep.first ||
        graph->getNode(ep.second->getId()) != ep.second) {
        std::cerr << "query nodes are not in the oracle graph." << std::endl;
        return {};
    }
    return shortest_path(ep);
}

template<bool bidirectional_graph>
query_result Oracle<bidirectional_graph>::query(model::Node *node1, model::Node *node2) {
    return query(model::endpoints{node1, node2});
}

template<bool bidirectional_graph>
double Oracle<bidirectional_graph>::similarity(model::Graph<bidirectional_graph> *graph) {
    auto other_graph = graph;

    double sse = 0;
    int edge_count = 0;

    for (auto other_id : other_graph->getEdgeIds()) {
        auto other_edge_o = other_graph->getEdge(other_id);
        auto our_edge_o = this->graph->getEdge(other_id);

        if (!other_edge_o || !our_edge_o)
            throw std::runtime_error("Topologies or edge ids mismatch.");

        auto other_edge = other_edge_o.value();
        auto our_edge = our_edge_o.value();

        sse += std::pow(other_edge->getEta() - our_edge->getEta(), 2);
        edge_count++;
    }

    return sse / edge_count;
}

template<bool bidirectional_graph>
model::Graph<bidirectional_graph> *Oracle<bidirectional_graph>::getGraph() const {
    return graph;
}

//namespace alg {
//    template class Oracle<true>;
//    template class Oracle<false>;
//}
