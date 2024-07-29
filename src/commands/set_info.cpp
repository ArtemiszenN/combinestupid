#include "set_info.h"

std::string Set_info::command_name = "setpayment";

void Set_info::set_info(std::shared_ptr<User_state> user_state, dpp::user user, std::string info) {
    user_state->set_user_info(user, info);
}

dpp::slashcommand Set_info::command(dpp::snowflake bot_id) {
    return dpp::slashcommand(Set_info::command_name, "Set payment details for yourself", bot_id)
        .add_option(dpp::command_option(
            dpp::co_string, "payment_info",
            "Your payment details. Don't put anything you don't want other people to see in here!", true));
}