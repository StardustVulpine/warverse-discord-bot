//
// Created by stardustvulpine on 4/17/26.
//

#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <nlohmann/json.hpp>
#include <stardustvulpine/Utils.hpp>

using json = nlohmann::json;
using Log = stardustvulpine::Utils::Console::Log;
using DiscordID = int64_t;

namespace wdb::db
{
    class DBManager
    {
        public:
        DBManager();
        ~DBManager() = default;

        [[nodiscard]] SQLite::Database &GetDatabase() const;

        void AddNewUser(std::string discordUsername, DiscordID discordUserID) const;
        void AddNewFraction(std::string name, const std::string& description, DiscordID discordRoleID) const;

        void RemoveUser(DiscordID discordUserID) const;
        void RemoveFraction(DiscordID discordRoleID) const;
        void RemoveAllUsers() const;
        void RemoveAllFractions() const;

        [[nodiscard]] nlohmann::json GetAllUsers() const;
        [[nodiscard]] std::string GetUserNameByID(int id) const;
        [[nodiscard]] nlohmann::json GetAllFractions() const;
        [[nodiscard]] std::string GetFractionNameByID(int id) const;

        private:
        std::filesystem::path m_DatabasePath;
        std::unique_ptr<SQLite::Database> m_Database{};

        void CreateDatabase() const;
        void CreateTables() const;
        void OpenDatabase();
        void BackupDatabase() const;
    };

    namespace error_code
    {
        constexpr int SQLITE_UNIQUE_CONSTRAINT = 19;
    }
} // wdb