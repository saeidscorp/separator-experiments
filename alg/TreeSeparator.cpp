//
// Created by saeid on 12/25/20.
//

#include "TreeSeparator.hpp"

using namespace alg;

template<bool bidirectional_graph>
TreeSeparator<bidirectional_graph>::TreeSeparator(Oracle<bidirectional_graph> *oracle)
        : Separator<bidirectional_graph>(oracle) {
    preprocess(oracle);
}

template<bool bidirectional_graph>
TreeSeparator<bidirectional_graph>::TreeSeparator(const model::Graph<bidirectional_graph> *graph)
        : LinearSeparator<bidirectional_graph>(new Oracle(graph)) {}
