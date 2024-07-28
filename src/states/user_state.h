#pragma once

#include <map>
#include <dpp/dpp.h>
#include <string>
#include <optional>
#include <iostream>
#include <thread>
#include <nlohmann/json.hpp>
#include <functional>
#include "../types/shared.h"
#include "../utils/io_utils.h"

class User_state
{
public:
    std::optional<user_id> get_user_info(user_id user);

    void set_user_info(dpp::user user, std::string info);

    void save();

    User_state();

    ~User_state();

private:
    std::string state_file = "user_state.txt";

    std::map<user_id, std::string> user_to_info;

    void load();
};