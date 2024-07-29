#include "commands/get_info.h"
#include "commands/help.h"
#include "commands/iou.h"
#include "commands/owes_me.h"
#include "commands/pay.h"
#include "commands/set_info.h"
#include "states/state_manager.h"
#include "states/tracker.h"
#include "states/user_state.h"
#include <dpp/dpp.h>
#include <memory>
#include <optional>
#include <string>
#include <thread>

// helper for std::visit
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

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
        std::unique_ptr<State_manager> state_manager = std::make_unique<State_manager>(60, 120, user_state, tracker);
        state_manager->start();
        bot.on_slashcommand([&user_state, &tracker](const dpp::slashcommand_t event) {
            dpp::user user = event.command.get_issuing_user();
            dpp::snowflake guild = event.command.get_guild().id;
            if (event.command.get_command_name() == Set_info::command_name) {
                std::string payment_info = std::get<std::string>(event.get_parameter("payment_info"));
                Set_info::set_info(user_state, user, payment_info);
                event.reply(dpp::message("Payment details set for " + (user.username) + " to:\n" + payment_info)
                                .set_flags(dpp::m_ephemeral));
            }
            if (event.command.get_command_name() == Get_info::command_name) {
                dpp::snowflake user_id = std::get<dpp::snowflake>(event.get_parameter("user"));
                std::string user_info =
                    Get_info::get_info(user_state, user_id.str()).value_or("No payment details available for user");
                event.reply(dpp::message("Payment details for " + dpp::user::get_mention(user_id) + ":\n" + user_info)
                                .set_flags(dpp::m_ephemeral));
            }
            if (event.command.get_command_name() == Owes_me::command_name) {
                dpp::snowflake owed = user.id;
                dpp::snowflake owes = std::get<dpp::snowflake>(event.get_parameter("user"));
                long double amount = std::get<double>(event.get_parameter("amount"));
                std::string comment = std::get<std::string>(event.get_parameter("comment"));
                std::variant<cents, transaction_error> add_payment_result =
                    Owes_me::owesme(tracker, guild.str(), owed.str(), owes.str(), amount);
                std::visit(overloaded{[&event, &owes, &owed, &amount, &comment](cents transferred) {
                                          event.reply(dpp::message(dpp::user::get_mention(owes) + " now owes " +
                                                                   dpp::user::get_mention(owed) + " an additional $" +
                                                                   std::to_string((long double)(transferred / 100.0)) +
                                                                   ". Reason:\n" + comment));
                                      },
                                      [&event](transaction_error error) {
                                          event.reply(dpp::message(error).set_flags(dpp::m_ephemeral));
                                      }},
                           add_payment_result);
            }
            if (event.command.get_command_name() == Iou::command_name) {
                dpp::snowflake user_id;
                try {
                    user_id = std::get<dpp::snowflake>(event.get_parameter("user"));
                } catch (const std::exception &e) {
                    user_id = user.id;
                }
                event.reply(dpp::message(Iou::iou(tracker, guild.str(), user_id.str())).set_flags(dpp::m_ephemeral));
            }
            if (event.command.get_command_name() == Pay::command_name) {
                dpp::snowflake from = user.id;
                dpp::snowflake to = std::get<dpp::snowflake>(event.get_parameter("user"));
                long double amount = std::get<double>(event.get_parameter("amount"));
                std::variant<cents, transaction_error> payment_result =
                    Pay::pay(tracker, guild.str(), from.str(), to.str(), amount);
                std::visit(overloaded{[&event, &from, &to, &amount](cents transferred) {
                                          event.reply(dpp::message(dpp::user::get_mention(from) + " paid " +
                                                                   dpp::user::get_mention(to) + " $" +
                                                                   std::to_string((long double)(transferred / 100.0))));
                                      },
                                      [&event](transaction_error error) {
                                          event.reply(dpp::message(error).set_flags(dpp::m_ephemeral));
                                      }},
                           payment_result);
            }
            if (event.command.get_command_name() == Help::command_name) {
                event.reply(dpp::message(Help::help_pretty_sprint()).set_flags(dpp::m_ephemeral));
            }
        });

        bot.on_ready([&bot](auto event) {
            if (dpp::run_once<struct register_bot_commands>()) {
                bot.global_bulk_command_create({Set_info::command(bot.me.id), Get_info::command(bot.me.id),
                                                Owes_me::command(bot.me.id), Iou::command(bot.me.id),
                                                Pay::command(bot.me.id), Help::command(bot.me.id)});
            }
        });

        bot.start(dpp::st_wait);
    } catch (const std::bad_optional_access &error) {
        std::cout << "Add bot token to BOT_TOKEN env\n";
    }

    return 0;
}