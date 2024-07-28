#include "tracker.h"

extern std::string state_file;
extern guild_adjacency_list adjacency_list;
extern guild_adjacency_list transposed_adjacency_list;

void Tracker::add_edge_naively(Tracker_edge edge)
{
    adjacency_list[edge.guild][edge.from][edge.to] += edge.amount;
    transposed_adjacency_list[edge.guild][edge.to][edge.from] += edge.amount;
}

void Tracker::remove_edge(guild_id guild, user_id from, user_id to)
{
    if (adjacency_list.contains(guild) && adjacency_list.at(guild).contains(from) && adjacency_list.at(guild).at(from).contains(to))
    {
        adjacency_list.at(guild).at(from).erase(adjacency_list.at(guild).at(from).find(to));
    }

    if (transposed_adjacency_list.contains(guild) && transposed_adjacency_list.at(guild).contains(to) && transposed_adjacency_list.at(guild).at(to).contains(from))
    {
        transposed_adjacency_list.at(guild).at(to).erase(transposed_adjacency_list.at(guild).at(to).find(from));
    }
}

std::optional<cents> Tracker::get_edge(guild_id guild, user_id from, user_id to)
{
    if (adjacency_list.contains(guild) && adjacency_list.at(guild).contains(from) && adjacency_list.at(guild).at(from).contains(to))
    {
        return adjacency_list.at(guild).at(from).at(to);
    }
    return {};
}

std::optional<Tracker_edge> Tracker::process_edge(Tracker_edge edge)
{
    auto [guild, from, to, amount] = edge;
    std::optional<cents> same_direction = Tracker::get_edge(guild, from, to);
    std::optional<cents> opposite_direction = Tracker::get_edge(guild, to, from);
    if (same_direction.has_value())
    {
        return edge;
    }
    else if (opposite_direction.has_value())
    {
        cents opposite_value = opposite_direction.value_or(0);
        if (opposite_value == amount)
        {
            remove_edge(guild, to, from);
            return {};
        }
        else if (opposite_value < amount)
        {
            remove_edge(guild, to, from);
            return Tracker_edge{guild, from, to, amount - opposite_value};
        }
        else
        {
            adjacency_list[edge.guild][edge.from][edge.to] -= edge.amount;
            transposed_adjacency_list[edge.guild][edge.to][edge.from] -= edge.amount;
            return {};
        }
    }
    else
    {
        return edge;
    }
}

void Tracker::add_edge_with_compression(Tracker_edge edge)
{
    std::optional<Tracker_edge> processed_edge = Tracker::process_edge(edge);
    processed_edge.transform([this](Tracker_edge lambda_edge)
                             { auto [guild, from, to, from_owes_to] = lambda_edge; 
                            if(Tracker::get_edge(guild, from, to).has_value()){
                                Tracker::add_edge_naively(lambda_edge);
                            }
                            else if (adjacency_list.contains(guild) && adjacency_list.at(guild).contains(to) && !adjacency_list.at(guild).at(to).empty()){
                                auto [third_party, to_add] = *adjacency_list.at(guild).at(to).begin();
                                Tracker::remove_edge(guild, to, third_party);
                                Tracker::add_edge_naively(Tracker_edge{guild, from, to, to_add + from_owes_to});
                            }else if(transposed_adjacency_list.contains(guild) && transposed_adjacency_list.at(guild).contains(from) && !transposed_adjacency_list.at(guild).at(from).empty()){
                                auto [third_party, third_party_owes_from] = *transposed_adjacency_list.at(guild).at(to).begin();
                                if(third_party_owes_from - from_owes_to == 0){
                                    Tracker::remove_edge(guild, third_party, from);
                                    Tracker::add_edge_naively(Tracker_edge{guild, third_party, to, third_party_owes_from});
                                }else if(third_party_owes_from - from_owes_to > 0){
                                    Tracker::remove_edge(guild, third_party, from);
                                    Tracker::add_edge_naively(Tracker_edge{guild, third_party, from, third_party_owes_from - from_owes_to});
                                    Tracker::add_edge_naively(Tracker_edge{guild, third_party, to, from_owes_to});
                                }else{
                                    Tracker::remove_edge(guild, third_party, from);
                                    Tracker::add_edge_naively(Tracker_edge{guild, from, to, from_owes_to - third_party_owes_from});
                                    Tracker::add_edge_naively(Tracker_edge{guild, third_party, to, third_party_owes_from});
                                }
                            } 
                            return lambda_edge; });
}

std::variant<bool, transaction_error> Tracker::add_transaction(guild_id guild, user_id to, user_id from, long double amount)
{
    if (amount > 1e15 || amount < -1e15 || (cents)amount <= 0)
    {
        return transaction_error("Amount entered is either above 1e15 dollars, nothing or a negative value");
    }
    Tracker::add_edge_with_compression(Tracker_edge{guild, to, from, (cents)amount});
    return true;
}

std::vector<Transaction> Tracker::get_transactions(guild_id guild, user_id user)
{
    std::vector<Transaction> transactions;
    if (adjacency_list.contains(guild) && adjacency_list.at(guild).contains(user) && !adjacency_list.at(guild).at(user).empty())
    {
        for (auto [to, amount] : adjacency_list.at(guild).at(user))
        {
            transactions.push_back(Transaction{to, amount, Owes});
        }
    }

    if (transposed_adjacency_list.contains(guild) && transposed_adjacency_list.at(guild).contains(user) && !transposed_adjacency_list.at(guild).at(user).empty())
    {
        for (auto [to, amount] : transposed_adjacency_list.at(guild).at(user))
        {
            transactions.push_back(Transaction{to, amount, Owed});
        }
    }
    return transactions;
}

void Tracker::save()
{
    std::cout << "Saving tracker state\n";
    nlohmann::json json;
    for (const auto &[guild, user_id_to_user_id_with_owed_amount] : adjacency_list)
    {
        for (const auto &[from, to_with_owed_amount] : user_id_to_user_id_with_owed_amount)
        {
            for (const auto &[to, owed_amount] : to_with_owed_amount)
            {
                json["adjacency_list"][guild][from][to] = owed_amount;
            }
        }
    }
    // this should be in io_utils, but passing json as argument to another translation unit is bugged
    std::thread writer([this, &json]()
                       {
        std::ofstream out(state_file);
        out << json;
        out.close(); });
    writer.join();
    std::cout << "Tracker state saved\n";
}

void Tracker::load()
{
    std::cout << "Loading tracker state\n";
    adjacency_list.clear();
    transposed_adjacency_list.clear();
    nlohmann::json json = Io_utils::read(state_file, "tracker_state");
    for (const auto &[guild, user_id_to_user_id_with_owed_amount] : json["adjacency_list"].items())
    {
        for (const auto &[from, to_with_owed_amount] : json["adjacency_list"][guild].items())
        {
            for (const auto &[to, owed_amount] : json["adjacency_list"][guild][from].items())
            {
                Tracker::add_edge_naively(Tracker_edge{guild, from, to, owed_amount});
            }
        }
    }
    std::cout << "Tracker state loaded successfully\n";
}

Tracker::Tracker()
{
    std::thread t(&Tracker::load, this);
    t.join();
}

Tracker::~Tracker()
{
    std::thread t(&Tracker::save, this);
    t.join();
}