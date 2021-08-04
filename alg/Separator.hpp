//
// Created by saeid on 12/25/20.
//

#ifndef SEPARATOR_SEPARATOR_HPP
#define SEPARATOR_SEPARATOR_HPP

#include "../model/model.hpp"
#include "Oracle.hpp"

#include "../util/stlutils.hpp"

namespace alg {

    template<bool bidirectional_graph>
    class Separator : public Oracle<bidirectional_graph>, public virtual model::Graph<bidirectional_graph> {
    protected:

        size_t _seps_count;

        float _avg_path_length;

        unsigned preprocessing_num_queries;

        std::vector<model::Node *> selected_nodes;

        std::map<int, bool> marked;

        // could've used a functor instead of this charade
        static constexpr auto table_comparator = [](model::endpoints const &lhs, model::endpoints const &rhs) {

            if constexpr (!bidirectional_graph) {
                return lhs < rhs;
            }

            auto lhs_min = std::min(lhs.first, lhs.second);
            auto lhs_max = std::max(lhs.first, lhs.second);
            auto rhs_min = std::min(rhs.first, rhs.second);
            auto rhs_max = std::max(rhs.first, rhs.second);

            return lhs_min < rhs_min || (!(rhs_min < lhs_min) && lhs_max < rhs_max);
        };

        typedef std::map<model::endpoints, model::ETA, decltype(table_comparator)> preprocessing_table;

        preprocessing_table table;

        virtual void preprocess(Oracle<bidirectional_graph> *oracle) = 0;

        template<int nth = 1>
        decltype(auto) closest_separator(model::Node *node) const {
            return util::min_by<nth>(selected_nodes, [node](const model::Node *n) {
                return model::Node::distance(node, n);
            });
        }

        [[nodiscard]] model::ETA eta_selectives(const model::path& selectives_path) const;

        [[nodiscard]] virtual model::path find_path(model::Node *from, model::Node *to) const = 0;

        [[nodiscard]] query_result do_query(model::endpoints ep) const override;

    public:

        explicit Separator(const model::Graph<bidirectional_graph> *graph);

        explicit Separator(Oracle<bidirectional_graph> *oracle);

        using Oracle<bidirectional_graph>::query;

        [[nodiscard]] double similarity() const;

        [[nodiscard]] double similarity_random() const;

        [[nodiscard]] unsigned preprocessing_queries() const;

        [[nodiscard]] double average_path_length() const;

        [[nodiscard]] std::string dotString() const override;
    };

    extern template class Separator<true>;
    extern template class Separator<false>;

}

#endif //SEPARATOR_SEPARATOR_HPP
