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
        static constexpr std::string VERSION = "0.0.1";

        dpp::cluster mBotCluster;

        Bot() : mBotCluster(Initialize(), dpp::i_default_intents)
        {
            SetBotLogger();
            m_dbManager = std::make_unique<db::DBManager>();

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

        std::unique_ptr<db::DBManager> *GetDBManager() {
            return &m_dbManager;
        }

        private:
        std::unique_ptr<db::DBManager> m_dbManager;
        commands::CommandManager mCommandManager;

        // Called as first method at start of the bot
        static std::string Initialize()
        {
            std::cout << "----------------------------------------" << std::endl;
            std::cout << "  Warverse Discord Bot - Version: " + VERSION << std::endl;
            std::cout << "----------------------------------------" << std::endl;

            Log::ToFile(Common::GetLogsDir());

            CreateDirectories();

            return GetToken();
        }

        void SetBotLogger()
        {
            Log::Trace("{} setting up bot logger...", __func__);

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

        static void CreateDirectories()
        {
            Log::Trace(__func__);
            if (!std::filesystem::exists(Common::GetAppDir())) {
                std::filesystem::create_directories(Common::GetAppDir());
            }
            if (!std::filesystem::exists(Common::GetLogsDir())) {
                std::filesystem::create_directories(Common::GetLogsDir());
            }
            if (!std::filesystem::exists(Common::GetDatabaseDir())) {
                std::filesystem::create_directories(Common::GetDatabaseDir());
            }
            if (!std::filesystem::exists(Common::GetImagesDir())) {
                std::filesystem::create_directories(Common::GetImagesDir());
            }
        }

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

        void OnCommandSentEvent();
        void OnMessageSentEventListen();
    };
}