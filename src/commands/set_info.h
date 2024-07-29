#pragma once

#include "../states/user_state.h"
#include <dpp/dpp.h>

namespace Set_info {
extern std::string command_name;
extern std::string help;
dpp::slashcommand command(dpp::snowflake bot_id);
void set_info(std::shared_ptr<User_state> user_state, dpp::user user, std::string info);
} // namespace Set_info