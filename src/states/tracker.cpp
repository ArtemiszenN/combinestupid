#include "tracker.h"

void Tracker::add_edge(Tracker_edge edge) {
    adjacency_list[edge.guild][edge.owed][edge.owes] += edge.amount;
    transposed_adjacency_list[edge.guild][edge.owes][edge.owed] += edge.amount;
}

void Tracker::add_edge_and_compress(Tracker_edge edge) {
    auto [guild, from, to, amount] = edge;
    std::lock_guard<std::mutex> guard(adjacency_list_mutex);
    Tracker::add_edge(edge);
    std::map<user_id, long long int> balances;
    for (auto [from, to_with_amount] : adjacency_list[guild]) {
        for (auto [to, amount] : adjacency_list[guild][from]) {
            balances[from] += amount;
            balances[to] -= amount;
        }
    }
    auto compare_by_amount = [](const std::pair<user_id, unsigned long long int> &lhs,
                                const std::pair<user_id, unsigned long long int> &rhs) {
        return lhs.second < rhs.second;
    };
    std::priority_queue<std::pair<user_id, unsigned long long int>,
                        std::vector<std::pair<user_id, unsigned long long int>>, decltype(compare_by_amount)>
        owes(compare_by_amount), owed(compare_by_amount);
    for (auto [user_id, amount] : balances) {
        if (amount < 0) {
            owes.push({user_id, (unsigned long long int)llabs(amount)});
        } else if (amount > 0) {
            owed.push({user_id, (unsigned long long int)amount});
        }
    }
    adjacency_list[guild].clear();
    transposed_adjacency_list[guild].clear();
    while (!owes.empty() || !owed.empty()) {
        auto [owes_user, owes_amount] = owes.top();
        owes.pop();
        auto [owed_user, owed_amount] = owed.top();
        owed.pop();
        if (owes_amount == owed_amount) {
            Tracker::add_edge(Tracker_edge{guild, owed_user, owes_user, owed_amount});
        } else if (owes_amount < owed_amount) {
            Tracker::add_edge(Tracker_edge{guild, owed_user, owes_user, owes_amount});
            owed.push({owed_user, owed_amount - owes_amount});
        } else {
            Tracker::add_edge(Tracker_edge{guild, owed_user, owes_user, owed_amount});
            owes.push({owes_user, owes_amount - owed_amount});
        }
    }
    std::cout << "Added transaction " << edge.owed << " is owed " << edge.amount << " by " << edge.owes << " in server "
              << edge.guild << '\n';
}

long long int Tracker::user_balance(guild_id guild, user_id user) {
    long long int user_balance = 0;
    if (adjacency_list.contains(guild) && adjacency_list.at(guild).contains(user)) {
        for (const auto &[owes, amount] : adjacency_list.at(guild).at(user)) {
            user_balance += amount;
        }
    }

    if (transposed_adjacency_list.contains(guild) && transposed_adjacency_list.at(guild).contains(user)) {
        for (const auto &[owed, amount] : transposed_adjacency_list.at(guild).at(user)) {
            user_balance -= amount;
        }
    }
    std::cout << "absolute balance for " << user << ": " << llabs((cents)user_balance) << '\n';
    return ((long long int)user_balance);
}

std::variant<cents, transaction_error> Tracker::add_transaction(guild_id guild, user_id owed, user_id owes,
                                                                long double amount) {
    if (user_balance(guild, owed) > 1e12) {
        return transaction_error("User being owed money is owed > $1e10 and cannot be owed any more money");
    }
    if (user_balance(guild, owes) < -1e12) {
        return transaction_error("User oweing money owes > $1e10 and cannot owe any more money");
    }
    if (amount > 1e10 || amount < -1e10 || (long long int)(amount * 100) <= 0) {
        return transaction_error("Amount entered is either above 1e10 dollars, nothing or a negative value");
    }
    std::cout << "Adding transaction " << owed << " is owed " << amount << " by " << owes << " in server " << guild
              << '\n';
    std::thread add_edge_job(&Tracker::add_edge_and_compress, this,
                             Tracker_edge{guild, owed, owes, (cents)(amount * 100)});
    add_edge_job.detach();
    return (cents)(amount * 100);
}

std::vector<Transaction> Tracker::get_transactions(guild_id guild, user_id user) {
    std::lock_guard<std::mutex> guard(adjacency_list_mutex);
    std::vector<Transaction> transactions;
    if (adjacency_list.contains(guild) && adjacency_list.at(guild).contains(user) &&
        !adjacency_list.at(guild).at(user).empty()) {
        for (auto [to, amount] : adjacency_list.at(guild).at(user)) {
            transactions.push_back(Transaction{to, amount, Owed});
        }
    }

    if (transposed_adjacency_list.contains(guild) && transposed_adjacency_list.at(guild).contains(user) &&
        !transposed_adjacency_list.at(guild).at(user).empty()) {
        for (auto [to, amount] : transposed_adjacency_list.at(guild).at(user)) {
            transactions.push_back(Transaction{to, amount, Owes});
        }
    }
    return transactions;
}

void Tracker::save() {
    std::lock_guard<std::mutex> guard(adjacency_list_mutex);
    std::cout << "Saving tracker state\n";
    nlohmann::json json;
    for (const auto &[guild, user_id_to_user_id_with_owed_amount] : adjacency_list) {
        for (const auto &[from, to_with_owed_amount] : user_id_to_user_id_with_owed_amount) {
            json["adjacency_list"][guild][from] = to_with_owed_amount;
        }
    }
    // this should be in io_utils, but passing json as argument to another translation unit is bugged
    std::ofstream out(state_file);
    out << json;
    out.close();
    std::cout << "Tracker state saved\n";
}

void Tracker::load() {
    std::cout << "Loading tracker state\n";
    adjacency_list.clear();
    transposed_adjacency_list.clear();
    nlohmann::json json = Io_utils::read(state_file, "tracker state");
    for (const auto &[guild, user_id_to_user_id_with_owed_amount] : json["adjacency_list"].items()) {
        for (const auto &[from, to_with_owed_amount] : json["adjacency_list"][guild].items()) {
            for (const auto &[to, owed_amount] : json["adjacency_list"][guild][from].items()) {
                Tracker::add_edge(Tracker_edge{guild, from, to, owed_amount});
            }
        }
    }
    std::cout << "Tracker state loaded successfully\n";
}

Tracker::Tracker() { Tracker::load(); }

Tracker::~Tracker() { Tracker::save(); }