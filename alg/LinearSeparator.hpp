//
// Created by saeid on 11/4/20.
//

#ifndef SEPARATOR_LINEARSEPARATOR_HPP
#define SEPARATOR_LINEARSEPARATOR_HPP

#include <map>

#include "Separator.hpp"
#include "../model/model.hpp"
#include "Oracle.hpp"

namespace alg {

    template<bool bidirectional_graph>
    class LinearSeparator : public Separator<bidirectional_graph>, public virtual Graph<bidirectional_graph> {
    protected:

        void preprocess(Oracle<bidirectional_graph> *oracle) override;

        using typename Separator<bidirectional_graph>::nodes_iterator;

        [[nodiscard]] ETA
        eta_selectives(nodes_iterator from_it,
                       nodes_iterator to_it)
        const override;

    public:

        explicit LinearSeparator(const model::Graph<bidirectional_graph> *graph);

        explicit LinearSeparator(Oracle<bidirectional_graph> *oracle);

        [[nodiscard]] std::string dotString() const override {
            return Separator<bidirectional_graph>::dotString();
        }

    };

}

#include "LinearSeparator.cpp"

#endif //SEPARATOR_LINEARSEPARATOR_HPP
