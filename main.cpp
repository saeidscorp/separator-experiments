
#include <iostream>
#include <chrono>

#include "model/model.hpp"
#include "gen/TreeGenerator.hpp"
#include "alg/TreeSeparator.hpp"
#include "gen/LinearGenerator.hpp"
#include "alg/LinearSeparator.hpp"
#include "alg/Oracle.hpp"

#include "util/printutils.hpp"
#include "util/progutils.hpp"

#include "magic_enum.hpp"

int main(int argc, const char **argv) {

    enum OperationMode {
        Linear, Tree, Planar
    } operation_mode = Linear;

    int n = 16;

    auto operation_mode_str = util::get_cmd_option(argc, argv, "-m");
    if (operation_mode_str) {
        auto parsed_mode = magic_enum::enum_cast<OperationMode>(operation_mode_str.value());
        if (parsed_mode.has_value())
            operation_mode = parsed_mode.value();
        else
            throw std::runtime_error("Invalid operation mode specified: " + operation_mode_str.value());
    }

    auto graph_size = util::get_cmd_option(argc, argv, "-n");
    if (graph_size)
        n = std::stoi(graph_size.value());

    if (operation_mode == Linear) {

        auto linear_gen = new gen::LinearGenerator<true>(n);
        auto rand_g     = linear_gen->generate();

        util::visualize_graph(rand_g);

        auto oracle = new alg::Oracle(rand_g);
        std::cout << "# Linear Separator:" << std::endl;
        auto linear_sep = new alg::LinearSeparator(rand_g);

        util::visualize_graph(linear_sep);

        std::cout << "Linear separator used " << linear_sep->preprocessing_queries() << " queries from the oracle"
                  << std::endl;
        std::cout << std::endl;

        auto                          start      = std::chrono::high_resolution_clock::now();
        auto                          similarity = linear_sep->similarity();
        std::chrono::duration<double> elapsed    = std::chrono::high_resolution_clock::now() - start;
        std::cout << ":: computing similarity took " << elapsed.count() << " seconds ::" << std::endl;

        std::cout << "MSE of linear separator-induced graph to the reference graph is: " << similarity << std::endl;

        start = std::chrono::high_resolution_clock::now();
        auto random_from_g = random_weights(rand_g);
        elapsed = std::chrono::high_resolution_clock::now() - start;
        std::cout << ":: randomizing weights took " << elapsed.count() << " seconds ::" << std::endl;

        start      = std::chrono::high_resolution_clock::now();
        similarity = oracle->similarity(random_from_g);
        elapsed    = std::chrono::high_resolution_clock::now() - start;
        std::cout << ":: computing similarity took " << elapsed.count() << " seconds" << std::endl;

        std::cout << "MSE of reference graph but with random weights to the reference itself is: "
                  << similarity << std::endl;
    }

    else if (operation_mode == Tree) {

        std::cout << "for tree of size " << n << std::endl;

        gen::TreeGenerator tree_gen{n, 2, 4};

        auto tree = tree_gen.generate();

        util::visualize_graph(tree);

        alg::TreeSeparator tree_sep{tree};

        util::visualize_graph<false>(&tree_sep);

        std::cout << "preprocessing queries: " << tree_sep.preprocessing_queries() << std::endl;
        std::cout << "similarity: " << tree_sep.similarity() << std::endl;
        // std::cout << "random similarity: " << tree_sep.similarity_random() << std::endl;
        std::cout << "average path length of queries: " << tree_sep.average_path_length() << std::endl;
    }

    else if (operation_mode == Planar) {
        std::cerr << "Not implemented yet." << std::endl;
    }

    return 0;
}

