
#include <iostream>
#include <chrono>

#include "model/model.hpp"
#include "gen/LinearGenerator.hpp"
#include "alg/LinearSeparator.hpp"
#include "gen/TreeGenerator.hpp"
#include "alg/CentroidDecomposition.hpp"
#include "alg/Oracle.hpp"

#include "util/stlutils.hpp"
#include "util/printutils.hpp"
#include "util/progutils.hpp"


int main(int argc, const char **argv) {

    gen::TreeGenerator tree_gen{16, 2, 4};
//    gen::LinearGenerator<true> linear_gen{4};
    auto graph = tree_gen.generate();
//    auto graph = linear_gen.generate();

    auto start = std::chrono::high_resolution_clock::now();
    alg::CentroidDecomposition centroid_decomposition{graph};
    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << ":: tree decomposing took " << elapsed.count() << " seconds ::" << std::endl;

    auto tree = centroid_decomposition.toGraph();

    util::visualize_graph(graph);
    util::visualize_graph(tree);

    return 0;
}

