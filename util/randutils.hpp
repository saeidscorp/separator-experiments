//
// Created by saeid on 12/23/20.
//

#ifndef SEPARATOR_RANDUTILS_HPP
#define SEPARATOR_RANDUTILS_HPP

#include "effolkronium/random.hpp"

#include "funcutils.hpp"

using Random = effolkronium::random_static;

namespace util {

    template<typename T>
    std::vector<T> number_partition(int parts, T sum) {

        std::vector<T> candidates;
        candidates.reserve(parts - 1);

        for (auto i = 0; i < parts - 1; ++i) {
            decltype(sum + parts) x;
            do
                x = Random::get(1, sum + parts - 1);
            while (std::any_of(candidates.cbegin(), candidates.cend(),
                               [x](auto elem) { return elem == x; }));
            candidates.push_back(x);
        }

        std::sort(candidates.begin(), candidates.end());
        candidates.push_back(sum + parts);

        std::vector<T> results;
        results.reserve(parts);

        std::adjacent_difference(candidates.cbegin(), candidates.cend(), std::back_inserter(results));
        std::for_each(results.begin(), results.end(), [](auto &x) { x--; });

        return results;
    }

}

#endif //SEPARATOR_RANDUTILS_HPP
