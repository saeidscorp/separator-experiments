//
// Created by saeid on 12/23/20.
//

#ifndef SEPARATOR_TREEGENERATOR_HPP
#define SEPARATOR_TREEGENERATOR_HPP

#include "../model/model.hpp"

namespace gen {

    class TreeGenerator {
    private:

        int _node_count;

        int _branching_min, _branching_max;

    public:

        TreeGenerator();

        explicit TreeGenerator(int node_count, int branching_min = 1, int branching_max = 2);

        model::Graph<true> *generate();

    };

}

#endif //SEPARATOR_TREEGENERATOR_HPP
