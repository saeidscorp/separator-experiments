//
// Created by saeid on 10/23/20.
//

#ifndef SEPARATOR_LINEARGENERATOR_HPP
#define SEPARATOR_LINEARGENERATOR_HPP

#include "../model/model.hpp"

namespace gen {

    template<bool bidirectional_graph>
    class LinearGenerator {
    private:

        int _length;

    public:
        LinearGenerator();

        LinearGenerator(int length);

        model::Graph<bidirectional_graph> *generate();
    };
}

#include "LinearGenerator.cpp"

#endif //SEPARATOR_LINEARGENERATOR_HPP
