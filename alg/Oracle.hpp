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

//    typedef std::function<ETA(model::endpoints)> query_function;

    template<bool bidirectional_graph>
    class Oracle {
    protected:

        unsigned num_queries;

        const model::Graph<bidirectional_graph> *graph;

        std::map<model::endpoints, int> frequencies;


        static query_result shortest_path(model::endpoints ep);

        [[nodiscard]] virtual query_result do_query(model::endpoints ep) const;

    public:

        explicit Oracle(const model::Graph<bidirectional_graph> *graph);

        query_result query(model::endpoints ep);

        query_result query(model::Node *node1, model::Node *node2);

        double similarity(const Oracle<bidirectional_graph> *other_oracle) const;

        double similarity(const model::Graph<bidirectional_graph> *other_graph) const;

        [[nodiscard]] model::Graph<bidirectional_graph> *getGraph() const;

        [[nodiscard]] unsigned queries() const;

    };
}

#include "Oracle.cpp"

#endif //SEPARATOR_ORACLE_HPP
