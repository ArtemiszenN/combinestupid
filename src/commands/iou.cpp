#include "iou.h"

std::string Iou::command_name = "iou";

std::string Iou::help = "See who owes a user money";

dpp::slashcommand Iou::command(dpp::snowflake bot_id) {
    return dpp::slashcommand(Iou::command_name, Iou::help, bot_id)
        .add_option(dpp::command_option(dpp::co_user, "user", "User whom you want to check", false));
}

std::string Iou::iou(std::shared_ptr<Tracker> tracker, guild_id guild, user_id user) {
    std::vector<Transaction> transactions = tracker->get_transactions(guild, user);
    std::string transactions_pretty_sprint =
        "Outstanding transactions for " + dpp::user::get_mention(dpp::snowflake(user)) + ":\n";
    for (Transaction transaction : transactions) {
        switch (transaction.transaction_type) {
        case Owed:
            transactions_pretty_sprint += dpp::user::get_mention(dpp::snowflake(transaction.other_party)) +
                                          " owes you $" + std::to_string((long double)transaction.amount / 100.0) +
                                          "\n";
            break;
        case Owes:
            transactions_pretty_sprint += "You owe $" + std::to_string((long double)transaction.amount / 100.0) +
                                          " to " + dpp::user::get_mention(dpp::snowflake(transaction.other_party)) +
                                          "\n";
            break;
        }
    }
    return transactions_pretty_sprint;
}