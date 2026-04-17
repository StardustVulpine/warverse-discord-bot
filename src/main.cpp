//
// Created by stardustvulpine on 3/26/26.
//

#include "Bot.hpp"

#include <SQLiteCpp/SQLiteCpp.h>
#include <filesystem>

std::string GetSQLQuery(const std::string& queryPath);

int main ()
{
    const SQLite::Database db("../../../db/test.db", SQLite::OPEN_READWRITE, -1);

    SQLite::Statement query(db, GetSQLQuery("../../../sql/query.sql"));

    while (query.executeStep())
    {
        int id = query.getColumn(0);
        std::string discordUsername = query.getColumn(1);
        int64_t discordUserID = query.getColumn(2);

        std::println(std::cout, "ID: {}, DiscordUsername: {}, DiscordUserID: {}", id, discordUsername, discordUserID);
    }

    warverse::Bot bot;
    bot.Start();

    return 0;
}

std::string GetSQLQuery(const std::string& queryPath)
{
    std::fstream fs(queryPath);
    const uintmax_t filesize = std::filesystem::file_size(queryPath);
    auto buffer = std::make_unique<char[]>(filesize + 1);
    fs.read(buffer.get(), static_cast<std::streamsize>(filesize));
    buffer[filesize] = '\0';

    return buffer.get();
}