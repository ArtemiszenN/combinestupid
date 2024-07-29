#include "get_info.h"

std::string Get_info::command_name = "getpayment";

std::optional<user_id> Get_info::get_info(std::shared_ptr<User_state> user_state, user_id user) {
    return user_state->get_user_info(user);
}
dpp::slashcommand Get_info::command(dpp::snowflake bot_id) {
    return dpp::slashcommand(Get_info::command_name, "Get payment details for another user", bot_id)
        .add_option(dpp::command_option(dpp::co_user, "user", "Whose payment details you want to see", true));
}