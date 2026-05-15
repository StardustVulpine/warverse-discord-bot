//
// Created by stardustvulpine on 4/8/26.
//
#pragma once
#include <dpp/dpp.h>
#include <nlohmann/json.hpp>
#include <stardustvulpine/Utils.hpp>
#include <DBManager.hpp>
#include <Common.hpp>
#include "CommandManager.hpp"

using json = nlohmann::json;
using Log = stardustvulpine::Utils::Console::Log;

namespace wdb
{
    class Bot
    {
        public:
        dpp::cluster mBotCluster;

        Bot() : mBotCluster(GetToken(), dpp::i_default_intents)
        {

            m_dbManager = std::make_unique<db::DBManager>();
            SetLogger();

            mBotCluster.on_ready([this](const dpp::ready_t&){
                mCommandManager.RegisterCommands(mBotCluster);
            });

            mBotCluster.on_slashcommand([this](const dpp::slashcommand_t& event) {
                mCommandManager.HandleIncomingCommand(event);
            });

        }

        ~Bot() = default;

        Bot(const Bot&) = delete;
        Bot(Bot&&) = delete;

        Bot& operator=(const Bot&) = delete;
        Bot& operator=(Bot&&) = delete;


        void Run();

        std::unique_ptr<db::DBManager> *GetDBManager()
        {
            return &m_dbManager;
        }

        private:
        std::unique_ptr<db::DBManager> m_dbManager;
        commands::CommandManager mCommandManager;

        static std::string GetToken()
        {
            Log::Trace("Bot::GetToken()");
            std::string tokenPath = std::format("{}/.token", Common::GetAppDir());
            if (!std::filesystem::exists(tokenPath))
            {
                Log::Warning("Token not found. Provide token first: ");
                std::string token;
                std::cin >> token;
                auto j = R"({ "token": "{}"})"_json;
                j["token"] = token;
                std::ofstream of(tokenPath);
                of << j;
                of.close();
                Log::Debug("Token saved to file.");
            }
            Log::Trace("Opening token file...");
            std::fstream f(tokenPath);
            if (!f.is_open())
            {
                Log::Warning("Token file could not be opened!");
                Log::Debug("Skipping token file...");
                return "";
            }
            Log::Trace("Token file opened!");
            json token_data = json::parse(f);
            f.close();
            Log::Info("Token Found!");
            return  token_data["token"];
        }

        void SetLogger()
        {
            Log::Trace("Bot::SetLogger() setting up bot logger...");

            mBotCluster.on_log([](const dpp::log_t& log)
            {
                switch (log.severity)
                {
                    case dpp::ll_info:
                        Log::Info(log.message);
                        break;
                    case dpp::ll_debug:
                        Log::Debug(log.message);
                        break;
                    case dpp::ll_trace:
                        Log::Trace(log.message);
                        break;
                    case dpp::ll_warning:
                        Log::Warning(log.message);
                        break;
                    case dpp::ll_error:
                        Log::Error(log.message);
                        break;
                    case dpp::ll_critical:
                        Log::Critical(log.message);
                        break;
                    default:
                        Log::Print(log.message);
                        break;
                }
            });
            Log::Trace("Logger set!");
        }
        void Commands();
        void MessageListeners();
    };
}