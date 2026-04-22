//
// Created by stardustvulpine on 4/17/26.
//

#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <stardustvulpine/Utils.hpp>

using Log = stardustvulpine::Utils::Console::Log;

namespace wdb::db
{
    class DBManager
    {
        public:
        DBManager();
        ~DBManager() = default;

        [[nodiscard]] SQLite::Database &GetDatabase() const;

        void CreateDatabase() const;
        void CreateTables() const;
        void OpenDatabase();

        void AddNewUser(std::string discordUsername, int64_t discordID) const;
        [[nodiscard]] std::string GetAllUsers() const;

        private:
        std::filesystem::path m_DatabasePath;
        std::unique_ptr<SQLite::Database> m_Database{};

        static std::string GetQueryFromSQLFile(const std::filesystem::path &queryPath)
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
    };
} // wdb