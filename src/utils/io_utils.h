#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace Io_utils
{
    nlohmann::json read(std::string file_path, std::string state_name);
}
