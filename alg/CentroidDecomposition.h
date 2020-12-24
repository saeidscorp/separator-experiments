//
// Created by saeid on 12/24/20.
//

#ifndef SEPARATOR_CENTROIDDECOMPOSITION_H
#define SEPARATOR_CENTROIDDECOMPOSITION_H

#include <vector>
#include <map>

#include "../model/model.hpp"

namespace alg {

    class CentroidDecomposition {
    private:

        std::map<model::Node *, model::Node *> _parent;

        std::map<model::Node *, std::size_t> _subtree_size;

        model::Node *root;

    public:

        explicit CentroidDecomposition(const model::Graph<true> *tree_graph);

        void build(const model::Graph<true> *graph);

        [[nodiscard]] model::Graph<false> *toGraph() const;
    };

}

#endif //SEPARATOR_CENTROIDDECOMPOSITION_H
