#pragma once

#include "../states/tracker.h"
#include "../types/shared.h"
#include "../types/tracker_types.h"
#include <dpp/dpp.h>
#include <string>
#include <vector>

namespace Iou {
extern std::string command_name;
extern std::string help;
dpp::slashcommand command(dpp::snowflake bot_id);
std::string iou(std::shared_ptr<Tracker> tracker, guild_id guild, user_id user);
} // namespace Iou