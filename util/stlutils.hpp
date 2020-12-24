//
// Created by saeid on 11/6/20.
//

#ifndef SEPARATOR_STLUTILS_HPP
#define SEPARATOR_STLUTILS_HPP

#include <map>
#include <vector>
#include <functional>
#include <concepts>

#include "funcutils.hpp"

namespace util {

    template<typename TK, typename TV>
    std::vector<TK> extract_keys(std::map<TK, TV> const &input_map) {
        std::vector<TK> retval;
        for (auto const &element : input_map) {
            retval.push_back(element.first);
        }
        return retval;
    }

    template<typename TK, typename TV>
    std::vector<TV> extract_values(std::map<TK, TV> const &input_map) {
        std::vector<TV> retval;
        for (auto const &element : input_map) {
            retval.push_back(element.second);
        }
        return retval;
    }


    template<typename TK, typename TV, SameTypeFunc F>
    requires requires (F f, TV v) {
        f(v);
    }
    std::pair<typename std::map<TK, TV>::iterator, bool> map_value_default(std::map<TK, TV> &map,
                                                                           TK const &key, const F lambda,
                                                                           TV const default_value = TV{}) {
        auto [it, inserted] = map.emplace(key, default_value);
        it->second = lambda(it->second);
        return {it, inserted};
    }


    template<typename T>
    concept ValueContainer = requires (T t) {
        typename T::value_type;
        t.begin();
        t.end();
    };

    template<int nth = 1, ValueContainer Container, UnaryFunc F, typename T = typename Container::value_type>
    requires requires (F f, T t) {
        f(t);
    }
    decltype(auto) min_by(const Container &container, const F lambda) {
        T sorted[nth];
        std::partial_sort_copy(container.begin(), container.end(),
                               sorted, sorted + nth,
                               [lambda](const auto &a, const auto &b) {
                                   return lambda(a) < lambda(b);
                               });
        return std::find(container.begin(), container.end(), sorted[nth - 1]);
    }

    template <class It>
    struct AdjacentRangeIt {
        It it_;

        auto operator*() const {
            return std::tie(*it_, *std::next(it_));
        }

        auto operator++() -> auto& { ++it_; return *this; }
        auto operator!=(AdjacentRangeIt other) const { return it_ != other.it_; }
    };

    template <class It>
    AdjacentRangeIt(It) -> AdjacentRangeIt<It>;

    template <class It, bool reverse = false, class FIt = It>
    struct AdjacentRange {
        It begin_, end_;

        template<class C>
        explicit AdjacentRange(const C& container){
            if constexpr (!reverse) {
                begin_ = std::begin(container);
                end_ = std::end(container);
            } else {
                begin_ = std::rbegin(container);
                end_ = std::rend(container);
            }
        }

        template<class C>
        AdjacentRange(const C& container, [[maybe_unused]] bool _discarded_) : AdjacentRange(container) {}

        AdjacentRange(FIt begin, FIt end) {
            if constexpr (!reverse) {
                begin_ = begin;
                end_ = end;
            } else {
                end_ = std::make_reverse_iterator(begin);
                begin_ = std::make_reverse_iterator(end);
            }
        }

        AdjacentRange(FIt begin, FIt end, [[maybe_unused]] bool _discarded_) : AdjacentRange(begin, end) {}

        auto begin() const {
            return AdjacentRangeIt{begin_};
        }
        auto end() const {
            if (begin_ == end_)
                return AdjacentRangeIt{end_};
            else
                return AdjacentRangeIt{std::prev(end_)};
        }
    };

    template<class C>
    AdjacentRange(C) -> AdjacentRange<typename C::const_iterator>;

    template<class C>
    AdjacentRange(C, bool) -> AdjacentRange<std::reverse_iterator<typename C::const_iterator>, true>;

    template<class It>
    AdjacentRange(It, It, bool) -> AdjacentRange<std::reverse_iterator<It>, true, It>;

    template<class It>
    AdjacentRange(It, It) -> AdjacentRange<It, false, It>;

    template<bool reverse = false, ValueContainer Container>
    auto adjacent_pairs(const Container &container) {
        if constexpr (reverse) {
            return AdjacentRange(container, reverse);
        } else {
            return AdjacentRange(container);
        }
    }

    template<bool reverse = false, typename It>
    auto adjacent_pairs(It begin, It end) {
        if constexpr (reverse) {
            return AdjacentRange(begin, end, reverse);
        } else {
            return AdjacentRange(begin, end);
        }
    }

}

#endif //SEPARATOR_STLUTILS_HPP
