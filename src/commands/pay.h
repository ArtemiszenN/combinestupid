#pragma once

#include "../states/tracker.h"
#include "../types/shared.h"
#include "../types/tracker_types.h"
#include <dpp/dpp.h>
#include <string>
#include <variant>

namespace Pay {
extern std::string command_name;
dpp::slashcommand command(dpp::snowflake bot_id);
std::variant<cents, transaction_error> pay(std::shared_ptr<Tracker> tracker, guild_id guild, user_id from, user_id to,
                                           long double amount);
} // namespace Pay