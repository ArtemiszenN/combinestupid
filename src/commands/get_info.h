#pragma once

#include "../states/user_state.h"
#include "../types/shared.h"
#include <dpp/dpp.h>
#include <optional>
#include <string>

namespace Get_info {
extern std::string command_name;
dpp::slashcommand command(dpp::snowflake bot_id);
std::optional<std::string> get_info(std::shared_ptr<User_state> user_state, user_id user);
} // namespace Get_info