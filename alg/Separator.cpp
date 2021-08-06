//
// Created by saeid on 12/25/20.
//

#include "Separator.hpp"

#include <sstream>

using namespace alg;

template<bool bidirectional_graph>
Separator<bidirectional_graph>::Separator(Oracle<bidirectional_graph> *oracle)
        : Oracle<bidirectional_graph>(oracle->getGraph()), _avg_path_length(0), preprocessing_num_queries(0) {

    auto nodes_count = oracle->getGraph()->getNodes().size();
    _seps_count = std::sqrt(std::max(nodes_count, 2ull) - 2) + 2;
}

template<bool bidirectional>
Separator<bidirectional>::Separator(const model::Graph<bidirectional> *graph)
        : Separator(new Oracle(graph)) {}

template<bool bidirectional_graph>
double Separator<bidirectional_graph>::similarity() const {
    Oracle<bidirectional_graph> oracle(this->getGraph());
    return oracle.similarity(static_cast<const Oracle<bidirectional_graph> *>(this));
}

template<bool bidirectional_graph>
double Separator<bidirectional_graph>::similarity_random() const {
    Oracle<bidirectional_graph> oracle(this->getGraph());
    return oracle.similarity_random(static_cast<const Oracle<bidirectional_graph> *>(this));
}

template<bool bidirectional_graph>
unsigned Separator<bidirectional_graph>::preprocessing_queries() const {
    return Separator<bidirectional_graph>::preprocessing_num_queries;
}

template<bool bidirectional_graph>
query_result Separator<bidirectional_graph>::do_query(model::endpoints ep) const {

    auto from = ep.first, to = ep.second;
    auto path = find_path(from, to);

    // IN PROGRESS TODO: develop path deduplication for reducing error

    auto sel_from = path.front(), sel_to = path.back();

    auto sel_eta = eta_selectives(path);

    // heuristic 1:
    auto sel2_from = *this->closest_separator(from, 2), sel2_to = *this->closest_separator(to, 2);

    // swap for deduplication
    if (sel_from == sel2_from) sel2_from = *this->closest_separator(from, 1);
    if (sel_to == sel2_to) sel2_to = *this->closest_separator(to, 1);

    // fixme: this breaks with unidirectional edges.
    auto eta_from_segment = eta_selectives(find_path(sel2_from, sel_from));
    auto eta_to_segment = eta_selectives(find_path(sel2_to, sel_to));
    auto ratio_from = model::Node::distance(from, sel_from)
                      / std::max(model::Node::distance(sel2_from, sel_from), 1.);
    auto ratio_to = model::Node::distance(to, sel_to)
                    / std::max(model::Node::distance(sel2_to, sel_to), 1.);

    auto heta_from = ratio_from * eta_from_segment, heta_to = ratio_to * eta_to_segment;
    auto total_eta = heta_from + sel_eta + heta_to;

    if (from != sel_from) path.push_front(from);
    if (to != sel_to) path.push_back(to);

    return model::path_length(path, total_eta);
}

template<bool bidirectional_graph>
model::ETA
Separator<bidirectional_graph>::eta_selectives(const model::path& selectives_path) const {

    auto accumulate = [this](auto begin, auto end) {
        return std::accumulate(begin, end, 0.,
                               [this](const auto &sum, const auto &pair) {
                                   const auto &[left, right] = pair;
                                   auto lookup = this->table.find({left, right});
                                   if (sum < 0 || lookup == this->table.end())
                                       return 0.;
                                   return sum + (*lookup).second;
                               });
    };

    if (selectives_path.empty())
        throw std::runtime_error("Input to `eta_selectives` must contain at least one selective node.");

    if (selectives_path.size() == 1)
        return 0;

    auto from_it = selectives_path.cbegin(), to_it = selectives_path.cend();
    auto adj_selected = util::adjacent_pairs(from_it, to_it);
    return accumulate(adj_selected.begin(), adj_selected.end());
}

template<bool bidirectional_graph>
std::string Separator<bidirectional_graph>::dotString() const {

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
    // ss << "    " << "rankdir=\"LR\";" << std::endl;

    for (auto &node : this->graph->getNodes()) {
        std::string *name = node->getName();
        if (name->empty()) continue;

        ss << "    " << node->getId();
        ss << " [label=\"" << *name << "\"";
        if (this->marked.at(node->getId()))
            ss << ", color=green";
        ss << "];" << std::endl;
    }

    for (auto &edge : this->graph->getEdges()) {
        ss << "    ";
        ss << edge->getFrom()->getId() << edge_specifier << edge->getTo()->getId();
        ss << std::endl;
    }

    ss << "}" << std::endl;
    return ss.str();
}

template<bool bidirectional_graph>
double Separator<bidirectional_graph>::average_path_length() const {
    return _avg_path_length;
}

namespace alg {
    template class Separator<true>;
    template class Separator<false>;
}
