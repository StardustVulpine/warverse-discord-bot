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
            "FractionRoleID" NUMERIC NOT NULL UNIQUE,
            "Name" TEXT,
            "Description" TEXT,
            "Level" INTEGER DEFAULT 0,
            "CurrentExp" NUMERIC DEFAULT 0,
            "ExpToNextLevel" NUMERIC DEFAULT 1000,
            "NextLevelExpRequirement_Mult" FLOAT DEFAULT 1.5,
            PRIMARY KEY ("FractionRoleID")
        );
    )";

    const std::string CREATE_USERS_TABLE = R"(
        CREATE TABLE "Users"
        (
            "DiscordID" NUMERIC NOT NULL UNIQUE,
            "DiscordUsername" TEXT NOT NULL,
            "FractionID" INTEGER,
            PRIMARY KEY("DiscordID"),
            FOREIGN KEY("FractionID") REFERENCES "Fractions"("FractionRoleID")
        );
    )";

    const std::string GET_ALL_USERS = "SELECT * FROM Users";
    const std::string GET_USERNAME_BY_ID = "SELECT DiscordUsername FROM Users WHERE DiscordID = ?";
    const std::string GET_ALL_FRACTIONS = "SELECT * FROM Fractions";
    const std::string GET_FRACTION_BY_ID = "SELECT Name FROM Fractions WHERE FractionRoleID = ?";

    const std::string ADD_NEW_DISCORD_USER = "INSERT INTO Users (DiscordUsername, DiscordID) VALUES (?, ?)";
    const std::string ADD_NEW_FRACTION = "INSERT INTO Fractions (Name, Description, FractionRoleID) VALUES (?, ?, ?)";

    const std::string DELETE_USERS = "DELETE FROM Users";
    const std::string DELETE_FRACTIONS = "DELETE FROM Fractions";

    const std::string DELETE_USER_BY_ID = "DELETE FROM Users WHERE DiscordID = ?";
    const std::string DELETE_FRACTION_BY_ID = "DELETE FROM Fractions WHERE FractionRoleID = ?";

}
