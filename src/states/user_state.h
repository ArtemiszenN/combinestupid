#pragma once
#include <unordered_map>
#include <dpp/dpp.h>
#include <string>
#include <optional>
#include <iostream>
#include <fstream>
#include <thread>
#include <future>
#include <nlohmann/json.hpp>
#include <functional>
class User_state
{
public:
    std::optional<std::string> get_user_info(dpp::user user);

    void set_user_info(dpp::user user, std::string info);
    void save();

    User_state();

    ~User_state();

private:
    std::string state_file = "user_state.txt";

    std::map<dpp::user, std::string, std::function<bool(const dpp::user &, const dpp::user &)>> user_to_info;

    void write(nlohmann::json json);

    nlohmann::json read();

    void load();
};