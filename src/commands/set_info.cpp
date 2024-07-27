#include "set_info.h"

void Set_info::set_info(std::shared_ptr<User_state> user_state, dpp::user user, std::string info)
{
    user_state->set_user_info(user, info);
}