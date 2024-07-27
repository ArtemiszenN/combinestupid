#pragma once

#include <dpp/dpp.h>
#include "../states/user_state.h"
#include <memory>
#include <thread>
class State_manager
{
public:
    void start();
    State_manager(int _save_interval_seconds, std::shared_ptr<User_state> _user_state);

private:
    int save_interval_seconds;
    std::shared_ptr<User_state> user_state;
};