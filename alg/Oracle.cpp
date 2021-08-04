//
// Created by saeid on 11/4/20.
//

#include "Oracle.hpp"

#include <map>
#include <queue>
#include <set>
#include <iostream>

#include "../util/stlutils.hpp"

#include "effolkronium/random.hpp"

using Random = effolkronium::random_static;

using namespace alg;

template<bool bidirectional_graph>
std::optional<model::path_length> Oracle<bidirectional_graph>::shortest_path(model::endpoints ep) {

    model::Node *start = ep.first, *goal = ep.second;

    auto heuristic = [&](model::Node *node) {
        return model::Node::distance(node, goal) / model::max_speed;
    };


    typedef std::map<model::Node *, model::ETA> score_map;

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
        model::ETA total_eta = 0;

        while (came_from.contains(current)) {
            auto prev_edge = came_from[current];
            current = prev_edge->getOther(current);
            total_path.push_front(current);
            total_eta += prev_edge->getEta();
        }

        return model::path_length(total_path, total_eta);
    };


    auto get_score = [](score_map *smap, model::Node *node) {
        auto it = smap->find(node);
        return it != smap->end() ? it->second : std::numeric_limits<model::ETA>::max();
    };


    while (!open_pq.empty()) {
        auto current = open_pq.top();

        if (current == goal)
            return reconstruct_path(current);

        open_pq.pop();

        for (auto neighbor : current->getNeighs()) {
            auto neighbor_edge = current->getEdgeOf(neighbor).value();
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
Oracle<bidirectional_graph>::Oracle(const model::Graph<bidirectional_graph> *graph) : num_queries(0), graph(graph) {}

template<bool bidirectional_graph>
query_result Oracle<bidirectional_graph>::do_query(model::endpoints ep) const {
    return shortest_path(ep);
}

template<bool bidirectional_graph>
query_result Oracle<bidirectional_graph>::query(model::endpoints ep) {
    num_queries++;
    util::map_value_default(frequencies, ep, [](int c) { return c + 1; });
    // todo: make this a part of Graph class and make it more robust
    if (graph->getNode(ep.first->getId()) != ep.first ||
        graph->getNode(ep.second->getId()) != ep.second) {
        std::cerr << "query nodes are not in the oracle graph." << std::endl;
        return {};
    }
    return do_query(ep);
}

template<bool bidirectional_graph>
query_result Oracle<bidirectional_graph>::query(model::Node *node1, model::Node *node2) {
    return query(model::endpoints{node1, node2});
}

template<bool bidirectional_graph>
double Oracle<bidirectional_graph>::similarity(const Oracle<bidirectional_graph> *other_oracle) const {
    auto ref_graph = this->getGraph();
    auto other_graph = other_oracle->getGraph();

    double sse = 0;
    unsigned query_count = 0;

    auto nodes = ref_graph->getNodes();
    auto start_index = 0ull, end_index = nodes.size();

    for (auto i = start_index; i < end_index; ++i)
        for (unsigned j = start_index; j < end_index; ++j) {

            if (i == j) continue;

            auto sep_eta = do_query({nodes[i], nodes[j]});
            if (!sep_eta)
                continue;

            auto from = other_graph->getNode(nodes[i]->getId()), to = other_graph->getNode(nodes[j]->getId());
            if (!from || !to)
                std::cerr << "node not available in second graph for similarity computation" << std::endl;
            auto oracle_eta = other_oracle->do_query({from.value(), to.value()});
            if (!oracle_eta)
                std::cerr << "oracle query unavailable" << std::endl;

            sse += std::pow(oracle_eta.value().second - sep_eta.value().second, 2);
            query_count++;
        }

    return sse / query_count;
}

template<bool bidirectional_graph>
model::Graph<bidirectional_graph> *Oracle<bidirectional_graph>::getGraph() const {
    return const_cast<model::Graph<bidirectional_graph> *>(graph);
}

template<bool bidirectional_graph>
unsigned Oracle<bidirectional_graph>::queries() const {
    return num_queries;
}

template<bool bidirectional_graph>
double Oracle<bidirectional_graph>::similarity(const model::Graph<bidirectional_graph> *other_graph) const {
    Oracle<bidirectional_graph> oracle(other_graph);
    return similarity(&oracle);
}

template<bool bidirectional_graph>
double Oracle<bidirectional_graph>::similarity_random(const Oracle<bidirectional_graph> *other_oracle) const {
    auto ref_graph = this->getGraph();
    auto other_graph = other_oracle->getGraph();

    double error_sum = 0;

    auto nodes = ref_graph->getNodes();
    const auto query_count = nodes.size();

    for (unsigned i = 0; i < query_count; ++i) {

        auto from_node = *Random::get(nodes);
        decltype(from_node) to_node;
        do to_node = *Random::get(nodes);
        while (to_node == from_node);

        auto sep_eta = do_query({from_node, to_node});
        if (!sep_eta) {
            i--;
            continue;
        }

        auto from = other_graph->getNode(from_node->getId()), to = other_graph->getNode(to_node->getId());
        if (!from || !to)
            std::cerr << "node not available in second graph for similarity computation" << std::endl;
        auto oracle_eta = other_oracle->do_query({from.value(), to.value()});
        if (!oracle_eta)
            std::cerr << "oracle query unavailable" << std::endl;

        error_sum += std::abs(oracle_eta.value().second - sep_eta.value().second);
    }

    return error_sum / query_count;
}

namespace alg {
    template class Oracle<true>;
    template class Oracle<false>;
}
