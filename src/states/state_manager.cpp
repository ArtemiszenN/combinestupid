#include "state_manager.h"
extern int interval;
extern std::shared_ptr<User_state> user_state;
State_manager::State_manager(int _save_interval_seconds, std::shared_ptr<User_state> _user_state) : save_interval_seconds(_save_interval_seconds), user_state(_user_state) {}
void State_manager::start()
{
    while (true)
    {
        user_state->save();
        std::this_thread::sleep_for(std::chrono::seconds(save_interval_seconds));
    }
}