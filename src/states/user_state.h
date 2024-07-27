class User_state
{
public:
    std::optional<std::string> get_user_info(dpp::user user);

    void set_user_info(dpp::user user, std::string info);

    User_state();

    ~User_state();
};