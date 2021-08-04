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

        explicit LinearGenerator(int length);

        model::Graph<bidirectional_graph> *generate();
    };

    extern template class LinearGenerator<true>;
    extern template class LinearGenerator<false>;

}

#endif //SEPARATOR_LINEARGENERATOR_HPP
