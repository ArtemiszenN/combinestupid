
#include "user_state.h"

extern std::string state_file;
extern std::map<std::string, std::string> user_to_info;

std::optional<std::string> User_state::get_user_info(dpp::user user)
{
    std::cout << "Getting user info for " << user.username << "\n";
    if (auto user_with_info = user_to_info.find(user.username); user_with_info != user_to_info.end())
    {
        return user_with_info->second;
    }
    return {};
}

void User_state::set_user_info(dpp::user user, std::string info)
{
    std::string username = user.username;
    user_to_info[username] = info;
    std::cout << "Setting payment info for " << username << " to " << info << '\n';
}

User_state::User_state()
{
    std::thread t(&User_state::load, this);
    t.join();
}

User_state::~User_state()
{
    std::thread t(&User_state::save, this);
    t.join();
}

void User_state::write(nlohmann::json json)
{
    std::cout << "Writing user state\n";
    std::ofstream out(state_file);
    out << json;
    out.close();
}

void User_state::save()
{
    std::cout << "Saving user state\n";
    nlohmann::json json;
    json["user_state"] = user_to_info;
    std::thread writer(&User_state::write, this, json);
    writer.join();
    std::cout << "User state saved\n";
}

nlohmann::json User_state::read()
{
    std::ifstream in(state_file);
    nlohmann::json json;

    try
    {
        json = nlohmann::json::parse(in);
        std::cout << "User state parsed successfully\n";
    }
    catch (std::exception parse_error)
    {
        std::cout << "Error occurred while parsing user state: " << parse_error.what() << '\n';
        json = nlohmann::json(std::map<nlohmann::json, std::string>());
        std::cout << "Resuming with empty state file\n";
    }

    in.close();
    return json;
}

void User_state::load()
{
    std::cout << "Loading user state\n";
    user_to_info.clear();
    nlohmann::json json = User_state::read();
    for (const auto &[username, info] : json["user_state"].items())
    {
        user_to_info[username] = info;
    }
    std::cout << "User state loaded successfully\n";
}
