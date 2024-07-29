#include "help.h"

std::string Help::command_name = "help";

std::string Help::help = "Prints this text";

dpp::slashcommand Help::command(dpp::snowflake bot_id) {
    return dpp::slashcommand(Help::command_name, "Look inside me @_@", bot_id);
}

std::string Help::help_pretty_sprint() {
    return Set_info::command_name + ": " + Set_info::help + "\n" + Get_info::command_name + ": " + Get_info::help +
           "\n" + Owes_me::command_name + ": " + Owes_me::help + "\n" + Pay::command_name + ": " + Pay::help + "\n" +
           Iou::command_name + ": " + Iou::help + "\n" + Help::command_name + ": " + Help::help + "\n";
}