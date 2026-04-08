//
// Created by stardustvulpine on 3/26/26.
//

#include <dpp/dpp.h>
#include <json.hpp>
#include "Bot.hpp"

using json = nlohmann::json;
std::string GetToken();

int main ()
{
    warverse::Bot bot(GetToken());
    bot.UpdateCommands();
    bot.RegisterCommands();
    bot.Start();

    return 0;
}

std::string GetToken()
{
    std::fstream f(".token");
    json token_data = json::parse(f);
    return token_data["token"];
}