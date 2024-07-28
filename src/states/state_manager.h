#pragma once

#include "tracker.h"
#include "user_state.h"
#include <dpp/dpp.h>
#include <memory>
#include <thread>

class State_manager {
  public:
    void start();

    State_manager(int _user_state_save_interval_seconds, int _tracker_save_interval_seconds,
                  std::shared_ptr<User_state> _user_state, std::shared_ptr<Tracker> _tracker);

    ~State_manager();

  private:
    int user_state_save_interval_seconds;

    int tracker_save_interval_seconds;

    std::shared_ptr<User_state> user_state;

    std::shared_ptr<Tracker> tracker;

    void start_user_state_saving();

    void start_tracker_saving();

    std::thread user_state_saver;

    std::thread tracker_saver;
};