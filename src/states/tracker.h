#pragma once

#include <dpp/dpp.h>
#include <map>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "../types/tracker_types.h"
#include "../utils/io_utils.h"
#include <vector>
#include <variant>

class Tracker
{
public:
    Tracker();

    ~Tracker();

    void save();

    // std::expected not being enabled even though header can be included, sad
    std::variant<bool, transaction_error> add_transaction(guild_id guild, user_id to, user_id from, long double amount);

    std::vector<Transaction> get_transactions(guild_id guild, user_id user);

private:
    std::string state_file = "tracker_state.txt";

    guild_adjacency_list adjacency_list;

    guild_adjacency_list transposed_adjacency_list;

    void add_edge_naively(Tracker_edge edge);

    void remove_edge(guild_id guild, user_id from, user_id to);

    std::optional<cents> get_edge(guild_id guild, user_id from, user_id to);

    std::optional<Tracker_edge> process_edge(Tracker_edge edge);

    void add_edge_with_compression(Tracker_edge edge);

    void load();
};