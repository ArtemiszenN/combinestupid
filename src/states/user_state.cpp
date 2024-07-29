#include "user_state.h"

extern std::string state_file;
extern std::map<user_id, std::string> user_to_info;

std::optional<user_id> User_state::get_user_info(user_id user) {
    std::cout << "Getting user info for " << user << "\n";
    if (auto user_with_info = user_to_info.find(user); user_with_info != user_to_info.end()) {
        return user_with_info->second;
    }
    return {};
}

void User_state::set_user_info(dpp::user user, std::string info) {
    user_to_info[user.id.str()] = info;
    std::cout << "Setting payment info for " << user.username << " to " << info << '\n';
}

User_state::User_state() { User_state::load(); }

User_state::~User_state() { User_state::save(); }

void User_state::save() {
    std::cout << "Saving user state\n";
    nlohmann::json json;
    json["user_state"] = user_to_info;
    // this should be in io_utils, but passing json as argument to another translation unit is bugged
    std::ofstream out(state_file);
    out << json;
    out.close();
    std::cout << "User state saved\n";
}

void User_state::load() {
    std::cout << "Loading user state\n";
    user_to_info.clear();
    nlohmann::json json = Io_utils::read(state_file, "user state");
    for (const auto &[user_id, info] : json["user_state"].items()) {
        user_to_info[user_id] = info;
    }
    std::cout << "User state loaded successfully\n";
}
