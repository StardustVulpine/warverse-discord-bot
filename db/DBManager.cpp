//
// Created by stardustvulpine on 4/17/26.
//

#include "DBManager.hpp"
#include "SQLQueries.hpp"

#include <fstream>
#include <filesystem>

#include <Common.hpp>
#include <dpp/user.h>
#include <dpp/nlohmann/json_fwd.hpp>

namespace wdb::db
{
    DBManager::DBManager()
    {
        Log::Trace(__func__);

        if (!std::filesystem::exists(Common::GetDatabaseDir())) {
            std::filesystem::create_directories(Common::GetDatabaseDir());
        }

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
        Log::Trace(__func__);
        return *m_Database;
    }

    void DBManager::CreateDatabase() const
    {
        Log::Trace(__func__);
        Log::Info("Creating database...");
        std::ofstream of(m_DatabasePath);
        of.close();
        Log::Info("Database created!");
    }

    void DBManager::CreateTables() const
    {
        Log::Trace(__func__);
        if (!m_Database->tableExists("Users"))
        {
            Log::Debug("Creating database 'Users' table...");
            SQLite::Statement(*m_Database, sql_queries::CREATE_USERS_TABLE).exec();
        }

        if (!m_Database->tableExists("Fractions"))
        {
            Log::Debug("Creating database 'Fractions' table...");
            SQLite::Statement(*m_Database, sql_queries::CREATE_FRACTIONS_TABLE).exec();
        }
        Log::Info("Database tables created!");
    }

    void DBManager::OpenDatabase()
    {
        Log::Trace(__func__);
        Log::Trace("Opening database...");
        BackupDatabase();
        m_Database = std::make_unique<SQLite::Database>(m_DatabasePath, SQLite::OPEN_READWRITE, -1);
        Log::Info("Database Loaded!");
    }

    void DBManager::BackupDatabase() const
    {
        Log::Trace(__func__);
        const std::filesystem::path backupPath = Common::GetDatabaseDir() + "/database.db.backup";
        std::filesystem::copy_file(m_DatabasePath, backupPath, std::filesystem::copy_options::overwrite_existing);
    }

    void DBManager::AddNewUser(std::string discordUsername, const int64_t discordUserID) const
    {
        Log::Trace(__func__);

        SQLite::Statement query(*m_Database, sql_queries::ADD_NEW_DISCORD_USER);
        query.bind(1, discordUsername);
        query.bind(2, discordUserID);

        Log::Trace("Query to be executed:\n{}", query.getExpandedSQL());
        query.exec();
        Log::Info("User {} added to database!", discordUsername);
    }

    void DBManager::AddNewFraction(std::string name, const std::string& description, const DiscordID discordRoleID) const
    {
        Log::Trace(__func__);

        SQLite::Statement query(*m_Database, sql_queries::ADD_NEW_FRACTION);
        query.bind(1, name);
        query.bind(2, description);
        query.bind(3, discordRoleID);

        Log::Trace("Query to be executed:\n{}", query.getExpandedSQL());
        query.exec();
        Log::Info("Fraction {} added to database successfully!", name);
    }

    void DBManager::RemoveUser(const DiscordID discordUserID) const
    {
        Log::Trace(__func__);
        SQLite::Statement query(*m_Database, sql_queries::DELETE_USER_BY_ID);
        query.bind(1, discordUserID);
        Log::Trace("Query to be executed:\n{}", query.getExpandedSQL());
        query.exec();
        Log::Info("User {} has been removed from database!", GetUserNameByID(discordUserID));
    }

    void DBManager::RemoveFraction(DiscordID discordRoleID) const
    {
        Log::Trace(__func__);
        SQLite::Statement query(*m_Database, sql_queries::DELETE_FRACTION_BY_ID);
        query.bind(1, discordRoleID);
        Log::Trace("Query to be executed:\n{}", query.getExpandedSQL());
        query.exec();
        Log::Info("Fraction {} has been removed from database!", GetFractionNameByID(discordRoleID));
    }

    void DBManager::RemoveAllUsers() const
    {
        Log::Trace(__func__);
        SQLite::Statement query(*m_Database, sql_queries::DELETE_USERS);
        Log::Trace("Query to be executed:\n{}", query.getExpandedSQL());
        query.exec();
        Log::Info("All Fractions have been removed from database!");
    }

    void DBManager::RemoveAllFractions() const
    {
        Log::Trace(__func__);
        SQLite::Statement query(*m_Database, sql_queries::DELETE_FRACTIONS);
        Log::Trace("Query to be executed:\n{}", query.getExpandedSQL());
        query.exec();
        Log::Info("All Users have been removed from database!");
    }

    nlohmann::json DBManager::GetAllUsers() const
    {
        Log::Trace(__func__);
        SQLite::Statement query(*m_Database, sql_queries::GET_ALL_USERS);
        Log::Trace("Query sent.");

        json results = json::array();

        while (query.executeStep())
        {
            nlohmann::json user;
            user["discordID"] = query.getColumn(0);
            user["discordUsername"] = query.getColumn(1);
            user["Fraction"] = GetFractionNameByID(query.getColumn(2));
            results.push_back(user);
        }
        Log::Trace("Users Json created!");
        Log::Trace(results.dump(4));
        return results;
    }

    std::string DBManager::GetUserNameByID(const DiscordID id) const
    {
        Log::Trace(__func__);
        SQLite::Statement query(*m_Database, sql_queries::GET_USERNAME_BY_ID);
        query.bind(1, id);

        while (query.executeStep()) {
            return std::string(query.getColumn(0));
        }
        return "";
    }

    bool DBManager::UserExists(const DiscordID id) const
    {
        if (GetUserNameByID(id).empty()) {
            return false;
        }
        return true;
    }

    nlohmann::json DBManager::GetAllFractions() const
    {
        Log::Trace(__func__);
        SQLite::Statement query(*m_Database, sql_queries::GET_ALL_FRACTIONS);
        Log::Trace("Query sent.");

        json results = json::array();

        while (query.executeStep())
        {
            nlohmann::json fraction;

            fraction["DiscordRoleID"] = query.getColumn(0);
            fraction["Name"] = query.getColumn(1);
            fraction["Description"] = query.getColumn(2);
            fraction["Level"] = query.getColumn(3);
            fraction["CurrentExp"] = query.getColumn(4);
            fraction["ExpToNextLevel"] = query.getColumn(5);

            results.push_back(fraction);
        }
        Log::Trace("Fractions Json created!");
        Log::Trace(results.dump(4));
        return results;
    }

    std::string DBManager::GetFractionNameByID(const DiscordID id) const
    {
        Log::Trace(__func__);
        SQLite::Statement query(*m_Database, sql_queries::GET_FRACTION_BY_ID);
        query.bind(1, id);

        while (query.executeStep()) {
            return std::string(query.getColumn(0));
        }
        return "";
    }

    bool DBManager::FractionExists(const DiscordID id) const
    {
        if (GetFractionNameByID(id).empty()) {
            return false;
        }
        return true;
    }
}


