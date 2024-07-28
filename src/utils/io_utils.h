#pragma once

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

namespace Io_utils {
nlohmann::json read(std::string file_path, std::string state_name);
}
