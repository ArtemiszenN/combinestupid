#pragma once

#include "../types/shared.h"
#include "../utils/io_utils.h"
#include <dpp/dpp.h>
#include <functional>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <thread>

class User_state {
  public:
    std::optional<user_id> get_user_info(user_id user);

    void set_user_info(dpp::user user, std::string info);

    void save();

    User_state();

    ~User_state();

  private:
    std::string state_file = "user_state.txt";

    std::map<user_id, std::string> user_to_info;

    std::mutex user_state_mutex;

    void load();
};