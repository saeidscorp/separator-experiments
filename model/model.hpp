//
// Created by saeid on 5/13/20.
//

#ifndef SEPARATOR_MODEL_HPP
#define SEPARATOR_MODEL_HPP

#include "Edge.hpp"
#include "Node.hpp"
#include "Graph.hpp"

#include <list>

namespace model {

    double const max_speed = 150;

    typedef std::list<Node *> path;

    typedef std::pair<path, ETA> path_length;

    typedef std::pair<Node *, Node *> endpoints;

    std::ostream &operator<< (std::ostream &o, Node const &node);
    std::ostream &operator<< (std::ostream &o, Node const *node);
    std::ostream &operator<< (std::ostream &o, std::string const *string);
}

#endif //SEPARATOR_MODEL_HPP
