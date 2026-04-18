//
// Created by stardustvulpine on 3/26/26.
//

#include "Bot.hpp"

int main ()
{
    /*const SQLite::Database db("../../../db/test.db", SQLite::OPEN_READWRITE, -1);

    SQLite::Statement query(db, GetSQLQuery("../../../sql/query.sql"));

    while (query.executeStep())
    {
        int id = query.getColumn(0);
        std::string discordUsername = query.getColumn(1);
        int64_t discordUserID = query.getColumn(2);

        std::println(std::cout, "ID: {}, DiscordUsername: {}, DiscordUserID: {}", id, discordUsername, discordUserID);
    }*/


    Log::ToFile();
    wdb::discord::Bot bot;
    bot.Start();

    return 0;
}
