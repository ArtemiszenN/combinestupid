#pragma once

#include "../types/tracker_types.h"
#include "../utils/io_utils.h"
#include <dpp/dpp.h>
#include <fstream>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <queue>
#include <string>
#include <variant>
#include <vector>

class Tracker {
  public:
    Tracker();

    ~Tracker();

    void save();

    // std::expected not being enabled even though header can be included, sad
    std::variant<cents, transaction_error> add_transaction(guild_id guild, user_id owed, user_id owes,
                                                           long double amount);

    std::vector<Transaction> get_transactions(guild_id guild, user_id user);

  private:
    std::string state_file = "tracker_state.txt";

    std::mutex adjacency_list_mutex;

    guild_adjacency_list adjacency_list;

    guild_adjacency_list transposed_adjacency_list;

    long long int user_balance(guild_id guild, user_id user);

    void add_edge(Tracker_edge edge);

    void add_edge_and_compress(Tracker_edge edge);

    void load();
};