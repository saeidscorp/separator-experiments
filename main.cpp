
#include <iostream>
#include <chrono>

#include "model/model.hpp"
#include "gen/LinearGenerator.hpp"
#include "alg/LinearSeparator.hpp"
#include "gen/TreeGenerator.hpp"
#include "alg/TreeSeparator.hpp"
#include "alg/Oracle.hpp"

#include "util/stlutils.hpp"
#include "util/printutils.hpp"
#include "util/progutils.hpp"


int main(int argc, const char **argv) {

//    gen::TreeGenerator tree_gen{16, 2, 4};
    gen::LinearGenerator<true> linear_gen;

//    alg::TreeSeparator tree_sep{tree_gen.generate()};
    alg::LinearSeparator linear_sep{linear_gen.generate()};

    std::cout << " preprocessing queries: " << linear_sep.preprocessing_queries() << std::endl;
    std::cout << " similarity: " << linear_sep.similarity() << std::endl;

    return 0;
}

