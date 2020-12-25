//
// Created by saeid on 12/25/20.
//

#include "Separator.hpp"

template<bool bidirectional_graph>
Separator<bidirectional_graph>::Separator(Oracle<bidirectional_graph> *oracle)
        : Oracle<bidirectional_graph>(oracle->getGraph()), preprocessing_num_queries(0), _avg_path_length(0) {

    auto nodes_count = oracle->getGraph()->getNodes().size();
    _seps_count = std::sqrt(std::max(nodes_count, 2ul) - 2) + 2;
}

template<bool bidirectional>
Separator<bidirectional>::Separator(const model::Graph<bidirectional> *graph)
        : Separator(new Oracle(graph)) {}

template<bool bidirectional_graph>
double Separator<bidirectional_graph>::similarity() const {
    Oracle<bidirectional_graph> oracle(this->getGraph());
    return oracle.similarity(this);
}

template<bool bidirectional_graph>
unsigned Separator<bidirectional_graph>::preprocessing_queries() const {
    return Separator<bidirectional_graph>::preprocessing_num_queries;
}

template<bool bidirectional_graph>
query_result Separator<bidirectional_graph>::do_query(model::endpoints ep) const {
    auto from = ep.first, to = ep.second;
    auto sel_from = this->closest_separator(from), sel_to = this->closest_separator(to);

    auto sel_eta = eta_selectives(sel_from, sel_to);

    // heuristic 1:
    auto sel2_from = this->closest_separator<2>(from), sel2_to = this->closest_separator<2>(to);

    // fixme: this breaks with unidirectional edges.
    auto eta_from_segment = eta_selectives(sel2_from, sel_from);
    auto eta_to_segment = eta_selectives(sel2_to, sel_to);
    auto ratio_from = model::Node::distance(from, *sel_from)
                      / std::max(model::Node::distance(*sel2_from, *sel_from), 1.);
    auto ratio_to = model::Node::distance(to, *sel_to)
                    / std::max(model::Node::distance(*sel2_to, *sel_to), 1.);

    auto heta_from = ratio_from * eta_from_segment, heta_to = ratio_to * eta_to_segment;
    auto total_eta = heta_from + sel_eta + heta_to;

    std::list<model::Node *> path;
    if (from != *sel_from) path.push_back(from);
    if (sel_from < sel_to)
        std::copy(sel_from, std::next(sel_to), std::back_inserter(path));
    else
        std::copy(std::reverse_iterator(std::next(sel_from)), std::reverse_iterator(sel_to),
                  std::back_inserter(path));
    if (to != *sel_to) path.push_back(to);

    return model::path_length(path, total_eta);
}
