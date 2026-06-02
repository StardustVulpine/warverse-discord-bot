//
// Created by stardustvulpine on 5/15/26.
//

#pragma once
#include <string>

namespace wdb::db::sql_queries
{
    const std::string CREATE_FRACTIONS_TABLE = R"(
        CREATE TABLE "Fractions"
        (
            "ID" INTEGER NOT NULL UNIQUE,
            "Name" TEXT,
            "Description" TEXT,
            "DiscordRoleID" NUMERIC NOT NULL UNIQUE,
            "CurrentExp" NUMERIC,
            "ExpToNextLevel" NUMERIC,
            "Level" INTEGER,
            PRIMARY KEY("ID" AUTOINCREMENT)
        );
    )";

    const std::string CREATE_USERS_TABLE = R"(
        CREATE TABLE "Users"
        (
            "ID" INTEGER NOT NULL UNIQUE,
            "DiscordUsername" TEXT NOT NULL,
            "DiscordID" NUMERIC NOT NULL UNIQUE,
            "FractionID" INTEGER,
            PRIMARY KEY("ID" AUTOINCREMENT),
            FOREIGN KEY("FractionID") REFERENCES "Fractions"("ID")
        );
    )";

    const std::string GET_ALL_USERS = "SELECT * FROM Users";
    const std::string GET_ALL_FRACTIONS = "SELECT * FROM Fractions";

    const std::string ADD_NEW_DISCORD_USER = "INSERT INTO Users (DiscordUsername, DiscordID) VALUES (?, ?)";
    const std::string ADD_NEW_FRACTION = "INSERT INTO Fractions (Name, Description, DiscordRoleID) VALUES (?, ?, ?)";

}
