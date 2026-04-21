//
// Created by stardustvulpine on 4/17/26.
//

#include "DBManager.hpp"
#include <stardustvulpine/Utils.hpp>
#include <Common.hpp>
#include <fstream>
#include <filesystem>

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

    SQLite::Database *DBManager::GetDatabase() const
    {
        return m_Database;
    }

    void DBManager::SetDatabase(SQLite::Database *db)
    {
        m_Database = db;
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
        m_Database = new SQLite::Database(m_DatabasePath, SQLite::OPEN_READWRITE, -1);
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

        std::string rplMsg = "Users:\n";

        while (query.executeStep())
        {
            int id = query.getColumn(0);
            std::string discordUsername = query.getColumn(1);
            int64_t discordUserID = query.getColumn(2);

            rplMsg += std::format("ID: {}, DiscordUsername: {}, DiscordUserID: {}\n", id, discordUsername, discordUserID);
        }
        return rplMsg;
    }
} // wdb