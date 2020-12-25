//
// Created by saeid on 11/4/20.
//

#ifndef SEPARATOR_LINEARSEPARATOR_HPP
#define SEPARATOR_LINEARSEPARATOR_HPP

#include <map>

#include "../model/model.hpp"
#include "Oracle.hpp"

namespace alg {

    template<bool bidirectional_graph>
    class LinearSeparator : public Oracle<bidirectional_graph> {
    protected:

        size_t _seps_count;

        float _avg_path_length;

        unsigned preprocessing_num_queries;

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

        typedef std::map<model::endpoints, ETA, decltype(table_comparator)> preprocessing_table;

        preprocessing_table table;

        void preprocess(Oracle<bidirectional_graph> *oracle);

        template<int nth = 1>
        decltype(auto) closest_separator(model::Node *node) const {
            return util::min_by<nth>(selected_nodes, [node](const model::Node *n) {
                return model::Node::distance(node, n);
            });
        }

        [[nodiscard]] virtual ETA eta_selectives(decltype(selected_nodes)::const_iterator from_it,
                                                 decltype(selected_nodes)::const_iterator to_it) const;

        [[nodiscard]] query_result do_query(model::endpoints ep) const override;

    public:

        explicit LinearSeparator(model::Graph<bidirectional_graph> *graph);

        explicit LinearSeparator(Oracle<bidirectional_graph> *oracle);

        using Oracle<bidirectional_graph>::query;

        [[nodiscard]] double similarity() const;

        [[nodiscard]] unsigned preprocessing_queries() const;

    };

}

#include "LinearSeparator.cpp"

#endif //SEPARATOR_LINEARSEPARATOR_HPP
