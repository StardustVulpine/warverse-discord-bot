//
// Created by stardustvulpine on 4/8/26.
//

#pragma once
#include <string>
#include <utility>
#include <dpp/dpp.h>

namespace warverse::commands
{
    class Command
    {
        public:
        std::string mName;
        std::string mDescription;

        Command(std::string name, std::string description) : mName(std::move(name)), mDescription(std::move(description))
        {
        }
    };
} // warverse