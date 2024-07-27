#include <dpp/dpp.h>
#include <string>
#include <optional>
#include <memory>
#include <thread>
#include "states/user_state.h"
#include "commands/set_info.h"
#include "states/state_manager.h"

std::optional<std::string> bot_token()
{
    char *token = std::getenv("BOT_TOKEN");
    if (token == nullptr)
    {
        return {};
    }
    return token;
}

int main()
{
    std::optional<std::string> token_result = bot_token();
    try
    {
        std::string token = token_result.value();
        dpp::cluster bot(token);
        std::cout << "Starting bot\n";
        std::shared_ptr<User_state> user_state = std::make_shared<User_state>();
        std::unique_ptr<State_manager> state_manager = std::make_unique<State_manager>(60, user_state);
        std::thread t([&state_manager]()
                      { state_manager->start(); });
        bot.on_slashcommand([&user_state](const dpp::slashcommand_t event)
                            {
        dpp::user user = event.command.get_issuing_user();
		if (event.command.get_command_name() == "setpayment") {
            std::string payment_info = std::get<std::string>(event.get_parameter("payment_info"));
            Set_info::set_info(user_state, user, payment_info);
            event.reply("Payment details set for " + (user.username) + " to:\n" + payment_info);
        } });

        bot.on_ready([&bot](auto event)
                     {
		if (dpp::run_once<struct register_bot_commands>()) {
            dpp::slashcommand setpayment = dpp::slashcommand("setpayment", "Set payment details for yourself", bot.me.id);
            setpayment.add_option(dpp::command_option(dpp::co_string, "payment_info", "payment_details", true));
            bot.global_bulk_command_create({setpayment});
        } });

        bot.start(dpp::st_wait);
    }
    catch (const std::bad_optional_access &error)
    {
        std::cout << "Add bot token to BOT_TOKEN env\n";
    }

    return 0;
}