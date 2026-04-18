//
// Created by stardustvulpine on 4/8/26.
//
#pragma once
#include <dpp/dpp.h>
#include <nlohmann/json.hpp>
#include <sqlite3.h>
#include <Log.hpp>

using json = nlohmann::json;
using Log = stardustvulpine::Utils::Console::Log;

namespace wdb::discord
{
    class Bot
    {
        public:
        dpp::cluster mBotCluster;

        Bot()
        {
            SetLogger();
            mBotCluster.token = GetToken();
        }
        explicit Bot(const std::string& token) : mBotCluster(token)
        {
            SetLogger();
        }

        ~Bot() = default;

        Bot(const Bot&) = delete;
        Bot(Bot&&) = delete;

        Bot& operator=(const Bot&) = delete;
        Bot& operator=(Bot&&) = delete;


        void Start();

        private:
        static std::string GetToken()
        {
            if (!std::filesystem::exists("token"))
            {
                Log::Print("Token not found. Provide token first: ");
                std::string token;
                std::cin >> token;
                auto j = R"({ "token": "{}"})"_json;
                j["token"] = token;
                std::ofstream of("token");
                of << j;
                of.close();
            }
            std::fstream f("token");
            if (!f.is_open())
            {
                std::println(std::cout, "Token file not found!");
                return "";
            }

            json token_data = json::parse(f);
            return  token_data["token"];
        }

        void SetLogger()
        {
            Log::ToFile();

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
        }
        void UpdateCommands();
        void RegisterCommands();

    };
}