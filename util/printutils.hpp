//
// Created by saeid on 12/2/20.
//

#ifndef SEPARATOR_PRINTUTILS_HPP
#define SEPARATOR_PRINTUTILS_HPP

#include <iostream>

#include "subprocess.hpp"

namespace util {

    void print_path(const Node *n1, const Node *n2, query_result &result) {
        if (result)
            std::cout << "eta from " << n1->getId() << " to " << n2->getId() << " is: " << result.value().second
                      << std::endl;
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

    template<bool bidirectional>
    void visualize_graph(const model::Graph<bidirectional> *graph) {

        subprocess::popen mktemp_process{"mktemp", {"-t", "tmp.separator.viz.XXXXXXXXXX"}};
        std::string temp_name{std::istreambuf_iterator(mktemp_process.stdout()), {}};
        temp_name.erase(std::remove_if(temp_name.begin(), temp_name.end(), ::isspace), temp_name.end());
        auto gv_file = temp_name + ".gv";
        auto image_file = temp_name + ".png";

        auto dot_string = graph->dotString();
        std::ofstream gv_stream(gv_file);
        gv_stream << dot_string;
        gv_stream.close();

        subprocess::popen dot{"dot", {"-Tpng", "-o", image_file, gv_file}};
        if (dot.wait())
            throw std::runtime_error("`dot` process failed.");

        std::system(("bash -c 'gwenview " + image_file
                     + " >/dev/null 2>&1 && rm " + image_file + " " + gv_file + " " + temp_name + "' &").c_str());
    }

}

#endif //SEPARATOR_PRINTUTILS_HPP
