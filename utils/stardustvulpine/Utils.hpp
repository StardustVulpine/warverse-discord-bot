//
// Created by stardustvulpine on 4/18/26.
//

#pragma once

#include "Log.hpp"

namespace stardustvulpine::Utils
{
    inline std::string GetUserDataDir()
    {
        char* userDataDir = getenv("XDG_DATA_HOME");
        if (!userDataDir)
        {
            userDataDir = getenv("HOME");
            return std::format("{}/.local/share", userDataDir);
        }
        userDataDir = getenv("XDG_DATA_HOME");
        return std::format("{}", userDataDir);
    }

    inline std::string PadString(std::string str, const size_t target_length) {
        if (str.length() < target_length) {
            str.resize(target_length, ' '); // Fill the rest with spaces
        } else if (str.length() > target_length) {
            str = str.substr(0, target_length - 3) + "..."; // Truncate if it's too long
        }
        return str;
    }
}

