//
// Created by saeid on 11/4/20.
//

#ifndef SEPARATOR_ORACLE_HPP
#define SEPARATOR_ORACLE_HPP

#include <list>
#include <optional>

#include "../model/model.hpp"

namespace alg {

    typedef std::optional<model::path_length> query_result;

    template<bool bidirectional_graph>
    class Oracle {
    private:

        int num_queries;

        model::Graph<bidirectional_graph> *graph;

        std::map<model::endpoints, int> frequencies;


        static query_result shortest_path(model::endpoints ep);

    public:

        explicit Oracle(model::Graph<bidirectional_graph> *graph);

        query_result query(model::endpoints ep);

        query_result query(model::Node *node1, model::Node *node2);

        double similarity(model::Graph<bidirectional_graph> *graph);

    };
}

#include "Oracle.cpp"

#endif //SEPARATOR_ORACLE_HPP
