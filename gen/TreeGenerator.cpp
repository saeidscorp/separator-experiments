//
// Created by saeid on 12/23/20.
//

#include "TreeGenerator.hpp"
#include "effolkronium/random.hpp"

#include "../util/randutils.hpp"

using Random = effolkronium::random_static;

using namespace gen;

TreeGenerator::TreeGenerator() : TreeGenerator(7) {}

TreeGenerator::TreeGenerator(int node_count, int branching_min, int branching_max)
        : _node_count(node_count), _branching_min(branching_min), _branching_max(branching_max) {}

model::Graph<true> *TreeGenerator::generate() {
    auto graph = new model::Graph<true>;

    auto dfs = util::recursive_lambda([&](auto &&dfs, model::Node *parent, int remaining, int current) {

        if (remaining == 0)
            return 0;

        auto new_node = graph->createNode(current, 0, std::to_string(current + 1));
        remaining--;

        if (parent != nullptr) {
            auto edge = graph->connect(parent, new_node);
            edge->setEta(Random::get(0., 1.));
        }

        auto children_count = Random::get(_branching_min, _branching_max);
        auto subtree_sizes = util::number_partition(children_count, remaining);

        auto count = 1;
        for (auto size : subtree_sizes)
            count += dfs(new_node, size, current + count);

        return count;
    });

    dfs(nullptr, _node_count, 0);

    return graph;
}
