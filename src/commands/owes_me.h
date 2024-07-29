#pragma once

#include "../states/tracker.h"
#include "../types/shared.h"
#include "../types/tracker_types.h"
#include <dpp/dpp.h>
#include <string>
#include <variant>

namespace Owes_me {
extern std::string command_name;
dpp::slashcommand command(dpp::snowflake bot_id);
std::variant<cents, transaction_error> owesme(std::shared_ptr<Tracker> tracker, guild_id guild, user_id owed,
                                              user_id owes, long double amount);
} // namespace Owes_me