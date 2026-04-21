//
// Created by stardustvulpine on 4/18/26.
//

#pragma once

#include <string>
#include <format>

#include <stardustvulpine/Utils.hpp>


namespace wdb::Common
{
    inline std::string GetAppDir()
    {
        return std::format("{}/warverse-discord-bot", stardustvulpine::Utils::GetUserDataDir());
    }
    inline std::string GetLogsDir()
    {
        return std::format("{}/logs", GetAppDir());
    }
    inline std::string GetDatabaseDir()
    {
        return std::format("{}/db", GetAppDir());
    }
}
