
#include <iostream>
#include <chrono>

#include "model/model.hpp"
#include "gen/TreeGenerator.hpp"
#include "alg/TreeSeparator.hpp"
#include "alg/Oracle.hpp"

#include "util/stlutils.hpp"
#include "util/printutils.hpp"
#include "util/progutils.hpp"


int main(int argc, const char **argv) {

    int n = 16;

    auto graph_size = get_cmd_option(argc, argv, "-n");
    if (graph_size)
        n = std::stoi(graph_size.value());

    std::cout << "for tree of size " << n << std::endl;

    gen::TreeGenerator tree_gen{n, 2, 4};

    alg::TreeSeparator tree_sep{tree_gen.generate()};

    util::visualize_graph<false>(&tree_sep);

    std::cout << "preprocessing queries: " << tree_sep.preprocessing_queries() << std::endl;
    std::cout << " similarity: " << tree_sep.similarity() << std::endl;

    return 0;
}

