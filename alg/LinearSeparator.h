//
// Created by saeid on 11/4/20.
//

#ifndef SEPARATOR_LINEARSEPARATOR_H
#define SEPARATOR_LINEARSEPARATOR_H

#include <map>

#include "../model/model.hpp"
#include "Oracle.hpp"

namespace alg {

    template<bool bidirectional_graph>
    class LinearSeparator {
    private:

        int _seps_count;

        float _avg_path_length;

        std::vector<model::Node *> selected_nodes;

        // could've used a functor instead of this charade
        static constexpr auto table_comparator = [](model::endpoints const& lhs, model::endpoints const& rhs) {

            if constexpr (!bidirectional_graph) {
                return lhs < rhs;
            }

            auto lhs_min = std::min(lhs.first, lhs.second);
            auto lhs_max = std::max(lhs.first, lhs.second);
            auto rhs_min = std::min(rhs.first, rhs.second);
            auto rhs_max = std::max(rhs.first, rhs.second);

            return lhs_min < rhs_min || (!(rhs_min < lhs_min) && lhs_max < rhs_max);
        };

        typedef std::map<model::endpoints, double, decltype(table_comparator)> preprocessing_table;

        preprocessing_table table;

        Oracle<bidirectional_graph> *oracle;

        model::Graph<bidirectional_graph> *graph;

        void preprocess();

    public:

        explicit LinearSeparator(model::Graph<bidirectional_graph> *graph);

        explicit LinearSeparator(Oracle<bidirectional_graph> *oracle);

    };

}

#include "LinearSeparator.cpp"

#endif //SEPARATOR_LINEARSEPARATOR_H
