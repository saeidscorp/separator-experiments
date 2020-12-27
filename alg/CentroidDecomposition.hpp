//
// Created by saeid on 12/24/20.
//

#ifndef SEPARATOR_CENTROIDDECOMPOSITION_HPP
#define SEPARATOR_CENTROIDDECOMPOSITION_HPP

#include <vector>
#include <map>

#include "../model/model.hpp"

namespace alg {

    class CentroidDecomposition : public model::Tree {
    private:

        template<bool bidirectional_graph>
        void build(const model::Graph<bidirectional_graph> *graph);

    public:

        template<bool bidirectional_graph>
        explicit CentroidDecomposition(const model::Graph<bidirectional_graph> *tree_graph);

    };

}

#endif //SEPARATOR_CENTROIDDECOMPOSITION_HPP
