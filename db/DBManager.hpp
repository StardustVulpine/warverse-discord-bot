//
// Created by stardustvulpine on 4/17/26.
//

#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <string>

namespace wdb::db
{
    class DBManager
    {
        public:
        DBManager();
        ~DBManager() = default;

        [[nodiscard]] SQLite::Database *GetDatabase() const;
        void SetDatabase(SQLite::Database *db);

        void CreateDatabase();
        void OpenDatabase();


        private:
        std::filesystem::path m_DatabasePath;
        SQLite::Database *m_Database{};

        static std::string GetQueryFromSQLFile(const std::filesystem::path &queryPath);
    };
} // wdb