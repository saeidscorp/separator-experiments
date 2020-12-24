//
// Created by saeid on 12/23/20.
//

#ifndef SEPARATOR_FUNCUTILS_HPP
#define SEPARATOR_FUNCUTILS_HPP

#include <functional>
#include <concepts>

namespace util {

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
    using second_arg_type = typename function_traits<T>::template arg<1>::type;

    template<typename T>
    concept UnaryFunc = function_traits<T>::arity == 1;

    template<typename T>
    concept BinaryFunc = function_traits<T>::arity == 2;


    template<typename T>
    concept UnitypeBinaryFunc = BinaryFunc<T> && std::is_same_v<first_arg_type<T>, second_arg_type<T>>;

    template<typename T>
    concept SameTypeFunc = UnaryFunc<T> && std::is_same_v<return_type<T>, first_arg_type<T>>;



    template <class F>
    struct y_combinator {
        F f; // the lambda will be stored here

        // a forwarding operator():
        template <class... Args>
        decltype(auto) operator()(Args&&... args) const {
            // we pass ourselves to f, then the arguments.
            // the lambda should take the first argument as `auto&& recurse` or similar.
            return f(*this, std::forward<Args>(args)...);
        }
    };

    // helper function that deduces the type of the lambda:
    template <class F>
    y_combinator<std::decay_t<F>> recursive_lambda(F&& f) {
        return {std::forward<F>(f)};
    }

}

#endif //SEPARATOR_FUNCUTILS_HPP
