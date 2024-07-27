#pragma once

#include <dpp/dpp.h>
#include "../states/user_state.h"
#include <memory>
namespace Set_info
{
    void set_info(std::shared_ptr<User_state> user_state, dpp::user user, std::string info);
}