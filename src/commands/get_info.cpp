#include "get_info.h"

std::optional<user_id> Get_info::get_info(std::shared_ptr<User_state> user_state, user_id user) {
    return user_state->get_user_info(user);
}
