#pragma once

#include "shared.h"
#include <map>

typedef unsigned long long int cents;
typedef std::map<user_id, std::map<user_id, cents>> user_id_to_user_id_with_owed_amount;
typedef std::map<guild_id, user_id_to_user_id_with_owed_amount> guild_adjacency_list;
typedef std::string transaction_error;

enum Transaction_type { Owes, Owed };

struct Tracker_edge {
    guild_id guild;
    user_id from;
    user_id to;
    cents amount;
};

struct Transaction {
    user_id to;
    cents amount;
    Transaction_type transaction_type;
};
