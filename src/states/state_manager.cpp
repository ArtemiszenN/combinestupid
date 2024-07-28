#include "state_manager.h"

extern int user_state_save_interval_seconds;
extern int tracker_save_interval_seconds;
extern std::shared_ptr<User_state> user_state;
extern std::shared_ptr<Tracker> tracker;
extern std::thread user_state_saver;
extern std::thread tracker_saver;

State_manager::State_manager(int _user_state_save_interval_seconds, int _tracker_save_interval_seconds,
                             std::shared_ptr<User_state> _user_state, std::shared_ptr<Tracker> _tracker)
    : user_state_save_interval_seconds(_user_state_save_interval_seconds), user_state(_user_state),
      tracker_save_interval_seconds(_tracker_save_interval_seconds), tracker(_tracker) {}

State_manager::~State_manager() {
    user_state_saver.join();
    tracker_saver.join();
}

void State_manager::start_user_state_saving() {
    std::this_thread::sleep_for(std::chrono::seconds(user_state_save_interval_seconds));
    while (true) {
        user_state->save();
        std::this_thread::sleep_for(std::chrono::seconds(user_state_save_interval_seconds));
    }
}

void State_manager::start_tracker_saving() {
    std::this_thread::sleep_for(std::chrono::seconds(tracker_save_interval_seconds));
    while (true) {
        tracker->save();
        std::this_thread::sleep_for(std::chrono::seconds(tracker_save_interval_seconds));
    }
}

void State_manager::start() {
    user_state_saver = std::thread(&State_manager::start_user_state_saving, this);
    tracker_saver = std::thread(&State_manager::start_tracker_saving, this);
}