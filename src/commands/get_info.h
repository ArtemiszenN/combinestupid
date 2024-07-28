#pragma once

#include "../states/user_state.h"
#include "../types/shared.h"
#include <dpp/dpp.h>
#include <memory>
#include <optional>
#include <string>

namespace Get_info {
std::optional<std::string> get_info(std::shared_ptr<User_state> user_state, user_id user);
}