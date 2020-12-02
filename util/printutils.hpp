//
// Created by saeid on 12/2/20.
//

#ifndef SEPARATOR_PRINTUTILS_HPP
#define SEPARATOR_PRINTUTILS_HPP

void print_path(const Node *n1, const Node *n2, query_result &result) {
    if (result)
        std::cout << "eta from " << n1->getId() << " to " << n2->getId() << " is: " << result.value().second << std::endl;
    else
        std::cout << "no path found" << std::endl;

    const auto separator = " -> ";
    const auto *sep = "";
    for (const auto &node : result.value().first) {
        std::cout << sep << node;
        sep = separator;
    }

    std::cout << std::endl;
}

#endif //SEPARATOR_PRINTUTILS_HPP
