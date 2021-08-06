//
// Created by saeid on 12/25/20.
//

#ifndef SEPARATOR_TREESEPARATOR_HPP
#define SEPARATOR_TREESEPARATOR_HPP

#include "../model/model.hpp"
#include "Separator.hpp"
#include "Oracle.hpp"

namespace alg {

    template<bool bidirectional_graph>
    class TreeSeparator : public Separator<bidirectional_graph>, public virtual model::Tree {
    protected:

        model::Tree decomposition;

        void preprocess(Oracle<bidirectional_graph> *oracle) override;

        [[nodiscard]] model::path find_path(model::Node *from, model::Node *to) const override;

        [[nodiscard]] model::Node * parent_in_decomposition(model::Node *node) const;

        [[nodiscard]] selected_iterator closest_separator(model::Node *node, int nth) const override;

    public:

        explicit TreeSeparator(const model::Graph<bidirectional_graph> *graph);

        explicit TreeSeparator(Oracle<bidirectional_graph> *oracle);

        [[nodiscard]] std::string dotString() const override {
            return Separator<bidirectional_graph>::dotString();
        }

        using Separator<bidirectional_graph>::similarity;

    };

    extern template class TreeSeparator<true>;
    extern template class TreeSeparator<false>;

}

#endif //SEPARATOR_TREESEPARATOR_HPP
