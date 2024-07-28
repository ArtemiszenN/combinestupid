#pragma once

#include <dpp/dpp.h>
#include "../states/user_state.h"
#include <memory>
#include <optional>
#include <string>
#include "../types/shared.h"

namespace Get_info
{
    std::optional<std::string> get_info(std::shared_ptr<User_state> user_state, user_id user);
}