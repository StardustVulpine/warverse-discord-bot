//
// Created by stardustvulpine on 4/18/26.
//

#pragma once

#include <string>
#include <format>
#include <dpp/dpp.h>
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
    inline std::string GetImagesDir()
    {
        return std::format("{}/img", GetAppDir());
    }

    inline bool IsMessageEmpty(const dpp::message& msg) {
        // A message is empty if ALL of these are true:
        return msg.content.empty() &&       // No standard text
               msg.embeds.empty() &&        // No embeds
               msg.attachments.empty() &&   // No uploaded attachment metadata
               msg.file_data.empty() &&   // No local files attached in memory
               msg.components.empty() &&    // No action rows (buttons/dropdowns)
               msg.stickers.empty();        // No stickers attached
    }
}
