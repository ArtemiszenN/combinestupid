#include "commands/get_info.h"
#include "commands/set_info.h"
#include "states/state_manager.h"
#include "states/tracker.h"
#include "states/user_state.h"
#include <dpp/dpp.h>
#include <memory>
#include <optional>
#include <string>
#include <thread>

std::optional<std::string> bot_token() {
    char *token = std::getenv("BOT_TOKEN");
    if (token == nullptr) {
        return {};
    }
    return token;
}

int main() {
    std::optional<std::string> token_result = bot_token();
    try {
        std::string token = token_result.value();
        dpp::cluster bot(token);
        std::cout << "Starting bot\n";
        std::shared_ptr<User_state> user_state = std::make_shared<User_state>();
        std::shared_ptr<Tracker> tracker = std::make_shared<Tracker>();
        std::unique_ptr<State_manager> state_manager = std::make_unique<State_manager>(60, 300, user_state, tracker);
        state_manager->start();
        bot.on_slashcommand([&user_state](const dpp::slashcommand_t event) {
            dpp::user user = event.command.get_issuing_user();
            if (event.command.get_command_name() == "setpayment") {
                std::string payment_info = std::get<std::string>(event.get_parameter("payment_info"));
                Set_info::set_info(user_state, user, payment_info);
                event.reply(dpp::message("Payment details set for " + (user.username) + " to:\n" + payment_info)
                                .set_flags(dpp::m_ephemeral));
            }
            if (event.command.get_command_name() == "getpayment") {
                dpp::snowflake user_id = std::get<dpp::snowflake>(event.get_parameter("user"));
                std::string user_info =
                    Get_info::get_info(user_state, user_id.str()).value_or("No payment details available for user");
                event.reply(dpp::message(user_info).set_flags(dpp::m_ephemeral));
            }
        });

        bot.on_ready([&bot](auto event) {
            if (dpp::run_once<struct register_bot_commands>()) {
                dpp::slashcommand setpayment =
                    dpp::slashcommand("setpayment", "Set payment details for yourself", bot.me.id);
                setpayment.add_option(dpp::command_option(
                    dpp::co_string, "payment_info",
                    "Your payment details. Don't put anything you don't want other people to see in here!", true));
                dpp::slashcommand getpayment =
                    dpp::slashcommand("getpayment", "Get payment details for another user", bot.me.id);
                getpayment.add_option(
                    dpp::command_option(dpp::co_user, "user", "Whose payment details you want to see", true));
                bot.global_bulk_command_create({setpayment, getpayment});
            }
        });

        bot.start(dpp::st_wait);
    } catch (const std::bad_optional_access &error) {
        std::cout << "Add bot token to BOT_TOKEN env\n";
    }

    return 0;
}