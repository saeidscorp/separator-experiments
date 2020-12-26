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

        using typename Separator<bidirectional_graph>::nodes_iterator;

        [[nodiscard]] ETA
        eta_selectives(nodes_iterator from_it,
                       nodes_iterator to_it)
        const override;

    public:

        explicit TreeSeparator(const model::Graph<bidirectional_graph> *graph);

        explicit TreeSeparator(Oracle<bidirectional_graph> *oracle);

        [[nodiscard]] std::string dotString() const override {
            return Separator<bidirectional_graph>::dotString();
        }

        using Separator<bidirectional_graph>::similarity;

    };

}

#include "TreeSeparator.cpp"

#endif //SEPARATOR_TREESEPARATOR_HPP
