//
// Created by saeid on 12/25/20.
//

#ifndef SEPARATOR_PROGUTILS_HPP
#define SEPARATOR_PROGUTILS_HPP

std::optional<std::string> get_cmd_option(const char ** begin, const char ** end, const std::string & option)
{
    const char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
        return *itr;
    return {};
}

std::optional<std::string> get_cmd_option(const int argc, const char **argv, const std::string & option)
{
    return get_cmd_option(argv, argv + argc, option);
}

#endif //SEPARATOR_PROGUTILS_HPP
