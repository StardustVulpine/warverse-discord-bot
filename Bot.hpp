//
// Created by stardustvulpine on 4/8/26.
//
#pragma once
#include <dpp/dpp.h>
#include <json.hpp>

using json = nlohmann::json;

namespace warverse
{
    class Bot
    {
        public:
        dpp::cluster mBotCluster;

        explicit Bot() : mBotCluster(GetToken())
        {
            mBotCluster.on_log(dpp::utility::cout_logger());
        }
        explicit Bot(const std::string& token) : mBotCluster(token)
        {
            mBotCluster.on_log(dpp::utility::cout_logger());
        }

        ~Bot() = default;

        Bot(const Bot&) = delete;
        Bot(Bot&&) = delete;

        Bot& operator=(const Bot&) = delete;
        Bot& operator=(Bot&&) = delete;

        void UpdateCommands();
        void RegisterCommands();
        void Start();

        private:
        static std::string GetToken()
        {
            std::fstream f(".token");
            json token_data = json::parse(f);
            return token_data["token"];
        }

    };
}