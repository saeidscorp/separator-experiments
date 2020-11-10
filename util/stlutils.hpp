//
// Created by saeid on 11/6/20.
//

#ifndef SEPARATOR_STLUTILS_HPP
#define SEPARATOR_STLUTILS_HPP

#include <map>
#include <vector>
#include <functional>
#include <concepts>

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

    template<typename T>
    struct function_traits
            : public function_traits<decltype(&T::operator())> {
    };
    // For generic types, directly use the result of the signature of its 'operator()'

    template<typename ClassType, typename ReturnType, typename... Args>
    struct function_traits<ReturnType(ClassType::*)(Args...) const>
    // we specialize for pointers to member function
    {
        enum {
            arity = sizeof...(Args)
        };
        // arity is the number of arguments.

        typedef ReturnType result_type;

        template<size_t i>
        struct arg {
            typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
            // the i-th argument is equivalent to the i-th tuple element of a tuple
            // composed of those arguments.
        };
    };

    template<typename T>
    using return_type = typename function_traits<T>::result_type;

    template<typename T>
    using first_arg_type = typename function_traits<T>::template arg<0>::type;

    template<typename T>
    concept SameTypeFunc = std::is_same_v<return_type<T>, first_arg_type<T>>;

    template<typename TK, typename TV, SameTypeFunc F>
    std::pair<typename std::map<TK, TV>::iterator, bool> map_value_default(std::map<TK, TV> &map,
                                                                           TK const &key, F lambda,
                                                                           TV const default_value = TV{}) {
        auto [it, inserted] = map.emplace(key, default_value);
        it->second = lambda(it->second);
        return {it, inserted};
    }

}

#endif //SEPARATOR_STLUTILS_HPP
