//
// Created by saeid on 12/2/20.
//

#ifndef SEPARATOR_PRINTUTILS_HPP
#define SEPARATOR_PRINTUTILS_HPP

#include <iostream>
#include <fstream>
#include <filesystem>

#include "subprocess.hpp"
#include "../alg/Oracle.hpp"

namespace util {

    template<typename _ = void>
    void print_path(const model::Node *n1, const model::Node *n2, alg::query_result &result) {
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
        using subprocess::CompletedProcess;
        using subprocess::PipeOption;
        namespace fs = std::filesystem;

#ifdef WIN32
        fs::path temp_dir{std::getenv("TEMP")};
        std::string temp_name{"tmp.separator.viz.XXXXXXXXXX"};
        mkstemp(temp_name.data());
        std::string temp_path = (temp_dir / temp_name).string();
#else
        auto process = subprocess::run({"mktemp", "-t", "tmp.separator.viz.XXXXXXXXXX"}, {
            .check = true,
            .cout = PipeOption::pipe
        });

        std::string temp_path{process.cout};
#endif

        auto gv_file = temp_path + ".gv";
        auto image_file = temp_path + ".png";

        auto dot_string = graph->dotString();
        std::ofstream gv_stream(gv_file);
        gv_stream << dot_string;
        gv_stream.close();

        auto dot_process = subprocess::run({"dot", "-Tpng", "-o", image_file, gv_file}, {
            .check = false,
            .cout = PipeOption::pipe
        });
        if (dot_process.returncode)
            throw std::runtime_error("`dot` process failed.");

#ifdef WIN32
        std::system(("start " + image_file).c_str());
#else
        std::system(("bash -c 'xdg-open " + image_file
                     + " >/dev/null 2>&1 && rm " + image_file + " " + gv_file + " " + temp_path + "' &").c_str());
#endif

    }

}

#endif //SEPARATOR_PRINTUTILS_HPP
