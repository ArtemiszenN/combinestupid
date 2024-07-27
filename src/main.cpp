#include <dpp/dpp.h>
#include <cstdlib>
#include <string>
#include <optional>
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

        bot.on_slashcommand([](const dpp::slashcommand_t event)
                            {
		if (event.command.get_command_name() == "ping") {
			event.reply("Pong!");
		} });

        bot.on_ready([&bot](auto event)
                     {
		if (dpp::run_once<struct register_bot_commands>()) {
			bot.global_command_create(
				dpp::slashcommand("ping", "Ping pong!", bot.me.id)
			);
		} });

        bot.start(dpp::st_wait);
    }
    catch (const std::bad_optional_access &error)
    {
        std::cout << "Add bot token to BOT_TOKEN env\n";
    }

    return 0;
}