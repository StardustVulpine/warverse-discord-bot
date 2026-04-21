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
}

