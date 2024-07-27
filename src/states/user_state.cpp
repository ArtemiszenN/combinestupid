
#include "user_state.h"
extern std::string state_file;

extern std::map<dpp::user, std::string, std::function<bool(const dpp::user &, const dpp::user &)>> user_to_info;
std::optional<std::string> User_state::get_user_info(dpp::user user)
{
    std::cout << "Getting user info for " << user.username << "\n";
    if (auto user_with_info = user_to_info.find(user); user_with_info != user_to_info.end())
    {
        return user_with_info->second;
    }
    return {};
}

void User_state::set_user_info(dpp::user user, std::string info)
{
    std::cout << "Setting payment info for " << user.username << " to " << info << '\n';
    user_to_info[user] = info;
}

User_state::User_state()
{
    user_to_info = std::map<dpp::user, std::string, std::function<bool(const dpp::user &, const dpp::user &)>>([](const dpp::user user, const dpp::user other)
                                                                                                               { return user.to_json(true) < other.to_json(true); });
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
    std::ofstream out(state_file);
    out << json;
    out.close();
}
void User_state::save()
{
    std::cout << "Saving user state\n";
    std::map<dpp::json, std::string> user_to_info_stringable;
    for (const auto &[dpp_user, info] : user_to_info)
    {
        user_to_info_stringable[dpp_user.to_json(true)] = info;
    }
    nlohmann::json json(user_to_info_stringable);
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
    std::future<nlohmann::json> reader = std::async([&]
                                                    { return User_state::read(); });
    nlohmann::json json = reader.get();
    for (const auto &[dpp_user, info] : json.items())
    {
        nlohmann::json json_user(dpp_user);
        dpp::user user(json_user);
        user_to_info[user] = info;
    }
    std::cout << "User state loaded successfully\n";
}
