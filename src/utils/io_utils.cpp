#include "io_utils.h"

nlohmann::json Io_utils::read(std::string file_path, std::string state_name)
{
    std::ifstream in(file_path);
    nlohmann::json json;

    try
    {
        json = nlohmann::json::parse(in);
        std::cout << state_name << " parsed successfully\n";
    }
    catch (std::exception parse_error)
    {
        std::cout << "Error occurred while parsing " << state_name << ": " << parse_error.what() << '\n';
        json = nlohmann::json(std::map<nlohmann::json, std::string>());
        std::cout << "Resuming with empty state file\n";
    }

    in.close();
    return json;
}