#include <unordered_map>
#include <dpp/dpp.h>
#include <string>
#include <optional>
#include <iostream>
#include <thread>
#include <future>
#include <nlohmann/json.hpp>

class User_state
{
public:
    std::optional<std::string> get_user_info(dpp::user user)
    {
        if (auto user_with_info = user_to_info.find(user); user_with_info != user_to_info.end())
        {
            return user_with_info->second;
        }
        return {};
    }

    void set_user_info(dpp::user user, std::string info)
    {
        user_to_info[user] = info;
    }
    User_state()
    {
        std::thread t(load);
        t.join();
    }
    ~User_state()
    {
        std::thread t(save);
        t.join();
    }

private:
    std::unordered_map<dpp::user, std::string> user_to_info;
    std::string state_file = "user_state.txt";
    void write(nlohmann::json json)
    {
        std::ofstream out(state_file);
        out << json;
        out.close();
    }
    void save()
    {
        std::unordered_map<dpp::json, std::string> user_to_info_stringable;
        for (const auto &[user, info] : user_to_info)
        {
            user_to_info_stringable[user.to_json(true)] = info;
        }
        nlohmann::json json(user_to_info_stringable);
        std::thread writer(write, json);
        writer.join();
    }
    nlohmann::json read()
    {
        std::ifstream in(state_file);
        nlohmann::json json = nlohmann::json::parse(in);
        in.close();
        return json;
    }
    void load()
    {
        user_to_info.clear();
        auto reader = std::async([&]
                                 { return read(); });
        nlohmann::json json = reader.get();
        for (const auto &[user, info] : json.items())
        {
            nlohmann::json json_user(user);
            dpp::user user(json_user);
            user_to_info[user] = info;
        }
    }
};