//
// Created by stardustvulpine on 18/2/26.
//

#pragma once
#include <format>
#include <string>
#include <iostream>
#include <chrono>
#include <filesystem>
#include <fstream>

namespace stardustvulpine::Console::Logger
{
    class Log
    {
        static constexpr std::string logDir = "./logs/";

        struct Color
        {
            std::string GREEN = "\033[38;2;0;255;1m";
            std::string YELLOW = "\033[38;2;255;191;0m";
            std::string ORANGE = "\033[38;2;204;85;0m";
            std::string RED = "\033[38;2;220;20;60m";
            std::string PURPLE = "\033[38;2;255;0;255m";
            std::string BLUE = "\033[38;2;0;255;255m";
            std::string RESET = "\033[0m";
        };

        enum class Severity
        {
            NONE_L = 0,
            INFO_L,
            DEBUG_L,
            TRACE_L,
            WARNING_L,
            ERROR_L,
            CRITICAL_L
        };

        public:
        static void ToFile()
        {
            if (!std::filesystem::exists(logDir))
            {
                std::filesystem::create_directory(logDir);
            }
            logFilePath = std::format("{}{}.log", logDir, Time());
            try
            {
                logFile.open(logFilePath);
                saveLogs = true;
            } catch (const std::exception& e)
            {
                Print("Could not create or open log file on the following path: {} \n Reason: {}", logFilePath.string(), e.what());
            }
        }

        template<class... Args> static void Print(const std::string_view msg, Args&&... args)
        {
            PrintLog(Severity::NONE_L, std::vformat(msg, std::make_format_args(args...)));
        }
        template<class... Args> static void Info(const std::string_view msg, Args&&... args)
        {
            PrintLog(Severity::INFO_L, std::vformat(msg, std::make_format_args(args...)));
        }

        template<class... Args> static void Debug(const std::string_view msg, Args&&... args)
        {
#ifdef DEBUG
            PrintLog(Severity::DEBUG_L, std::vformat(msg, std::make_format_args(args...)));
#endif
        }

        template<class... Args> static void Trace(const std::string_view msg, Args&&... args)
        {
#ifdef DEBUG
            PrintLog(Severity::TRACE_L, std::vformat(msg, std::make_format_args(args...)));
#endif
        }

        template<class... Args> static void Warning(const std::string_view msg, Args&&... args)
        {
            PrintLog(Severity::WARNING_L, std::vformat(msg, std::make_format_args(args...)));
        }

        template<class... Args> static void Error(const std::string_view msg, Args&&... args)
        {
            PrintLog(Severity::ERROR_L, std::vformat(msg, std::make_format_args(args...)));
        }

        template<class... Args> static void Critical(const std::string_view msg, Args&&... args)
        {
            PrintLog(Severity::CRITICAL_L, std::vformat(msg, std::make_format_args(args...)));
        }

        private:
        static inline bool saveLogs;
        static inline std::filesystem::path logFilePath;
        static inline std::ofstream logFile;
        static const Color color;

        static void PrintLog(const Severity severity, const std::string& msg)
        {
            std::string col;
            std::string level;
            switch (severity)
            {
                case Severity::INFO_L:
                    col = color.GREEN;
                    level = "INFO";
                    break;
                case Severity::DEBUG_L:
                    col = color.RESET;
                    level = "DEBUG";
                    break;
                case Severity::TRACE_L:
                    col = color.BLUE;
                    level = "TRACE";
                    break;
                case Severity::WARNING_L:
                    col = color.YELLOW;
                    level = "WARNING";
                    break;
                case Severity::ERROR_L:
                    col = color.ORANGE;
                    level = "ERROR";
                    break;
                case Severity::CRITICAL_L:
                    col = color.RED;
                    level = "CRITICAL";
                    break;
                case Severity::NONE_L:
                    col = color.RESET;
                    level = "";
                    break;
            }
            const auto logTime = Time();
            auto logString = std::format("{}[{}] {}: {}{}", col, logTime, level, msg, color.RESET);
            std::println(std::cout,"{}", logString);

            if (saveLogs)
            {
                logString = std::format("[{}] {}: {}\n", logTime, level, msg);
                logFile.write(logString.c_str(), logString.size());
                logFile.flush();
            }
        }

        static std::string Time()
        {
            const auto utc_now{std::chrono::system_clock::now()};
            const auto time = std::chrono::current_zone()->to_local(utc_now);

            return std::format("{}", time);
        }
    };
    inline const Log::Color Log::color = Color();
}
