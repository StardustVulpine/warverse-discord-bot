//
// Created by stardustvulpine on 4/17/26.
//

#include "DBManager.hpp"

#include <fstream>
#include <filesystem>

#include <stardustvulpine/Utils.hpp>
#include <Common.hpp>
#include <dpp/user.h>
#include <dpp/nlohmann/json_fwd.hpp>
#include <dpp/nlohmann/json_fwd.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace wdb::db
{
    DBManager::DBManager()
    {
        Log::Trace("DBManager::DBManager()");
        m_DatabasePath = Common::GetDatabaseDir() + "/database.db";
        Log::Trace("Database file path: {}", m_DatabasePath.string());
        if (!std::filesystem::exists(m_DatabasePath)) {
            Log::Warning("Database file not found!");
            CreateDatabase();
        }
        OpenDatabase();
        if (!m_Database->tableExists("Users") || !m_Database->tableExists("Fractions"))
        {
            Log::Warning("Database tables not found!");
            CreateTables();
        }
    }

    SQLite::Database &DBManager::GetDatabase() const
    {
        return *m_Database;
    }

    void DBManager::CreateDatabase() const
    {
        Log::Info("Creating database...");
        std::filesystem::create_directories(m_DatabasePath.parent_path());
        Log::Debug("Database directory created...");
        std::ofstream of(m_DatabasePath);
        of.close();
        Log::Info("Database created!");
    }

    void DBManager::CreateTables() const
    {
        if (!m_Database->tableExists("Users"))
        {
            Log::Debug("Creating database 'Users' table...");
            SQLite::Statement(*m_Database, GetQueryFromSQLFile("../../../db/sql/createUsersTable.sql")).exec();
        }

        if (!m_Database->tableExists("Fractions"))
        {
            Log::Debug("Creating database 'Fractions' table...");
            SQLite::Statement(*m_Database, GetQueryFromSQLFile("../../../db/sql/createFractionsTable.sql")).exec();
        }
        Log::Info("Database tables created!");
    }

    void DBManager::OpenDatabase()
    {
        Log::Trace("Opening database...");
        m_Database = std::make_unique<SQLite::Database>(m_DatabasePath, SQLite::OPEN_READWRITE, -1);
        Log::Info("Database Loaded!");
    }

    void DBManager::AddNewUser(std::string discordUsername, int64_t discordID) const
    {
        Log::Trace("DBManager::AddNewUser()");

        std::string query = std::format(
            R"(INSERT INTO Users (DiscordUsername, DiscordID) VALUES ('{}', '{}'))",
            discordUsername, discordID);
        Log::Trace("Query to be executed:\n{}", query);
        SQLite::Statement(*m_Database, query).exec();
    }

    std::string DBManager::GetAllUsers() const
    {
        Log::Trace("DBManager::GetAllUsers()");
        SQLite::Statement query(*m_Database, "SELECT * FROM Users");
        Log::Trace("Query sent.");

        int userCount = 0;
        nlohmann::ordered_json usersAll;

        while (query.executeStep())
        {
            userCount++;
            Log::Trace("Getting user: {}", userCount);

            nlohmann::ordered_json user;

            user["ID"] = query.getColumn(0);
            user["discordUsername"] = query.getColumn(1);
            user["discordID"] = query.getColumn(2);
            user["Fraction"] = GetFractionNameByID(query.getColumn(3));

            Log::Debug("Got all user's fields from database. Preparing json object...");

            const std::string userKey = std::format("User {}", userCount);

            usersAll[userKey] = user;
        }
        Log::Trace("Json created!");
        Log::Debug(usersAll.dump(4));
        return usersAll.dump(4);
    }

    std::string DBManager::GetFractionNameByID(int id) const
    {
        SQLite::Statement query(*m_Database, std::format("SELECT Name FROM Fractions WHERE ID={}", id));
        std::string fractionName;
        while (query.executeStep())
        {
            fractionName = std::string(query.getColumn(0));
        }
        return fractionName;
    }

    std::string DBManager::GetAllFractions() const
    {
        Log::Trace("DBManager::GetAllFractions()");
        SQLite::Statement query(*m_Database, "SELECT * FROM Fractions");
        Log::Trace("Query sent.");

        int fractionCount = 0;
        nlohmann::ordered_json fractionsAll;

        while (query.executeStep())
        {
            fractionCount++;
            Log::Trace("Getting user: {}", fractionCount);

            nlohmann::ordered_json fraction;

            fraction["ID"] = query.getColumn(0);
            fraction["Name"] = query.getColumn(1);
            fraction["Description"] = query.getColumn(2);
            fraction["CurrentExp"] = query.getColumn(3);
            fraction["ExpToNextLevel"] = query.getColumn(4);
            fraction["Level"] = query.getColumn(4);

            Log::Debug("Got all fraction's fields from database. Preparing json object...");

            const std::string fractionKey = std::format("Fraction {}", fractionCount);

            fractionsAll[fractionKey] = fraction;
        }
        Log::Trace("Json created!");
        Log::Debug(fractionsAll.dump(4));
        return fractionsAll.dump(4);
    }
}
