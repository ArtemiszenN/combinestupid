#include "pay.h"

std::string Pay::command_name = "pay";

std::string Pay::help = "Record that you paid someone else money";

std::variant<cents, transaction_error> Pay::pay(std::shared_ptr<Tracker> tracker, guild_id guild, user_id from,
                                                user_id to, long double amount) {
    return tracker->add_transaction(guild, from, to, amount);
};

dpp::slashcommand Pay::command(dpp::snowflake bot_id) {
    return dpp::slashcommand(Pay::command_name, Pay::help, bot_id)
        .add_option(dpp::command_option(dpp::co_user, "user", "Who owes you money", true))
        .add_option(dpp::command_option(dpp::co_number, "amount",
                                        "Amount, in the agreed upon currency that the other party owes you", true));
}