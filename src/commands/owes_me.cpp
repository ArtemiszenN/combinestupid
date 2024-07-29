#include "owes_me.h"

std::string Owes_me::command_name = "owesme";

std::variant<cents, transaction_error> Owes_me::owesme(std::shared_ptr<Tracker> tracker, guild_id guild, user_id owed,
                                                       user_id owes, long double amount) {
    return tracker->add_transaction(guild, owed, owes, amount);
};

dpp::slashcommand Owes_me::command(dpp::snowflake bot_id) {
    return dpp::slashcommand(Owes_me::command_name, "Record that someone else owes you money", bot_id)
        .add_option(dpp::command_option(dpp::co_user, "user", "Who owes you money", true))
        .add_option(dpp::command_option(dpp::co_number, "amount",
                                        "Amount, in the agreed upon currency that the other party owes you", true))
        .add_option(dpp::command_option(dpp::co_string, "comment", "Why you are now owed money", true));
}