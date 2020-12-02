
#include <iostream>

#include "model/model.hpp"
#include "gen/LinearGenerator.hpp"
#include "alg/LinearSeparator.h"
#include "alg/Oracle.hpp"
#include "util/stlutils.hpp"
#include "util/printutils.hpp"

int main() {

    auto linear_gen = new gen::LinearGenerator<true>;
    auto rand_g = linear_gen->generate();
    std::cout << rand_g->dotString() << std::endl;

    auto nodes_ids = rand_g->getNodeIds();
    auto n1 = rand_g->getNode(nodes_ids[3]).value();
    auto n2 = rand_g->getNode(nodes_ids[nodes_ids.size() - 3]).value();

    std::cout << "# Oracle:" << std::endl;
    auto oracle = new alg::Oracle(rand_g);
    auto result = oracle->query(n1, n2);
    print_path(n1, n2, result);

    std::cout << std::endl;

    std::cout << "# Linear Separator:" << std::endl;
    auto linear_sep = new alg::LinearSeparator(rand_g);
    result = linear_sep->query(n1, n2);
    print_path(n1, n2, result);

    std::cout << std::endl;

    return 0;
}

