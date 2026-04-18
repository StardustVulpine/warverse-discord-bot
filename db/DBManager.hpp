//
// Created by stardustvulpine on 4/17/26.
//

#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <filesystem>
#include <fstream>
#include <string>

namespace wdb::db
{
    class DBManager
    {
        public:
        static void OpenDatabase()
        {

        }


        private:
        SQLite::Database db;

        static std::string GetQueryFromSQLFile(const std::string& queryPath)
        {
            std::fstream fs(queryPath);
            const uintmax_t filesize = std::filesystem::file_size(queryPath);
            auto buffer = std::make_unique<char[]>(filesize + 1);
            fs.read(buffer.get(), static_cast<std::streamsize>(filesize));
            buffer[filesize] = '\0';

            return buffer.get();
        }

    };
} // wdb