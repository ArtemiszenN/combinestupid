#pragma once

#include "get_info.h"
#include "iou.h"
#include "owes_me.h"
#include "pay.h"
#include "set_info.h"
#include <dpp/dpp.h>
#include <string>

namespace Help {
extern std::string command_name;
extern std::string help;
dpp::slashcommand command(dpp::snowflake bot_id);
std::string help_pretty_sprint();
} // namespace Help