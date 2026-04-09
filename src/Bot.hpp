//
// Created by stardustvulpine on 4/8/26.
//
#pragma once
#include <dpp/dpp.h>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

using json = nlohmann::json;

namespace warverse
{
    class Bot
    {
        public:
        dpp::cluster mBotCluster;

        Bot() : mBotCluster(GetToken())
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
            std::fstream f("token.json");
            if (!f.is_open())
            {
                std::println(std::cout, "Token file not found!");
                return "";
            }

            json token_data = json::parse(f);
            return  token_data["token"];
        }

    };
}