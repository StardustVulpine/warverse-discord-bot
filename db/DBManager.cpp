//
// Created by stardustvulpine on 4/17/26.
//

#include "DBManager.hpp"
#include <stardustvulpine/Utils.hpp>
#include <Common.hpp>
#include <fstream>
#include <filesystem>

using Log = stardustvulpine::Utils::Console::Log;

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
            return;
        }
        OpenDatabase();
    }

    SQLite::Database *DBManager::GetDatabase() const
    {
        return m_Database;
    }

    void DBManager::SetDatabase(SQLite::Database *db)
    {
        m_Database = db;
    }


    void DBManager::CreateDatabase()
    {
        Log::Info("Creating database...");
        std::filesystem::create_directories(m_DatabasePath.parent_path());
        Log::Debug("Database directory created...");
        std::ofstream of(m_DatabasePath);
        of.close();
        Log::Info("Database created!");

        OpenDatabase();

        Log::Debug("Creating database tables...");
        SQLite::Statement(*m_Database, GetQueryFromSQLFile("../../../db/sql/createUsersTable.sql")).exec();
        SQLite::Statement(*m_Database, GetQueryFromSQLFile("../../../db/sql/createFractionsTable.sql")).exec();
        Log::Info("Database tables created!");
    }

    void DBManager::OpenDatabase()
    {
        Log::Trace("Opening database...");
        m_Database = new SQLite::Database(m_DatabasePath, SQLite::OPEN_READWRITE, -1);
        Log::Info("Database Loaded!");
    }

    std::string DBManager::GetQueryFromSQLFile(const std::filesystem::path &queryPath)
    {
        Log::Trace("DBManager::GetQueryFromSQLFile() with path: {}", queryPath.string());
        std::fstream fs(queryPath);
        const uintmax_t filesize = std::filesystem::file_size(queryPath);
        auto buffer = std::make_unique<char[]>(filesize + 1);
        fs.read(buffer.get(), static_cast<std::streamsize>(filesize));
        buffer[filesize] = '\0';
        Log::Trace("Query to be executed:\n{}", buffer.get());

        return buffer.get();
    }
} // wdb