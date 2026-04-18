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
#include "Common.hpp"

namespace stardustvulpine::Utils::Console
{
    class Log
    {
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
            std::string logDir = GetLogsDir();
            std::filesystem::create_directories(logDir);

            logFilePath = std::format("{}/{}.log", logDir, Time());
            try
            {
                logFile.open(logFilePath);
                saveLogs = true;
                Info("Logging to: {}", logFilePath.string());
            } catch (const std::exception& e)
            {
                Error("Could not create or open log file on the following path: {} \n Reason: {}", logFilePath.string(), e.what());
            }
        }

        template<class... Args> static void Print(const std::string_view msg, Args&&... args)
        {
            std::string message;
            if (sizeof ... (args) == 0)
            {
                message = std::string(msg);
            }
            else
            {
                message = std::vformat(msg, std::make_format_args(args...));
            }
            PrintLog(Severity::NONE_L, message);
        }
        template<class... Args> static void Info(const std::string_view msg, Args&&... args)
        {
            std::string message;
            if (sizeof ... (args) == 0)
            {
                message = std::string(msg);
            }
            else
            {
                message = std::vformat(msg, std::make_format_args(args...));
            }
            PrintLog(Severity::INFO_L, message);
        }
        template<class... Args> static void Debug(const std::string_view msg, Args&&... args)
        {
#ifdef DEBUG
            std::string message;
            if (sizeof ... (args) == 0)
            {
                message = std::string(msg);
            }
            else
            {
                message = std::vformat(msg, std::make_format_args(args...));
            }
            PrintLog(Severity::DEBUG_L, message);
#endif
        }

        template<class... Args> static void Trace(const std::string_view msg, Args&&... args)
        {
#ifdef DEBUG
            std::string message;
            if (sizeof ... (args) == 0)
            {
                message = std::string(msg);
            }
            else
            {
                message = std::vformat(msg, std::make_format_args(args...));
            }
            PrintLog(Severity::TRACE_L, message);
#endif
        }

        template<class... Args> static void Warning(const std::string_view msg, Args&&... args)
        {
            std::string message;
            if (sizeof ... (args) == 0)
            {
                message = std::string(msg);
            }
            else
            {
                message = std::vformat(msg, std::make_format_args(args...));
            }
            PrintLog(Severity::WARNING_L, message);
        }

        template<class... Args> static void Error(const std::string_view msg, Args&&... args)
        {
            std::string message;
            if (sizeof ... (args) == 0)
            {
                message = std::string(msg);
            }
            else
            {
                message = std::vformat(msg, std::make_format_args(args...));
            }
            PrintLog(Severity::ERROR_L, message);
        }

        template<class... Args> static void Critical(const std::string_view msg, Args&&... args)
        {
            std::string message;
            if (sizeof ... (args) == 0)
            {
                message = std::string(msg);
            }
            else
            {
                message = std::vformat(msg, std::make_format_args(args...));
            }
            PrintLog(Severity::CRITICAL_L, message);
        }

        private:
        static inline bool saveLogs;
        static inline std::filesystem::path logFilePath;
        static inline std::ofstream logFile;

        static void PrintLog(const Severity severity, const std::string& msg)
        {
            std::string GREEN = "\033[38;2;0;255;1m";
            std::string YELLOW = "\033[38;2;255;191;0m";
            std::string ORANGE = "\033[38;2;204;85;0m";
            std::string RED = "\033[38;2;220;20;60m";
            std::string PURPLE = "\033[38;2;255;0;255m";
            std::string BLUE = "\033[38;2;0;255;255m";
            std::string RESET_COLOR = "\033[0m";

            std::string col;
            std::string level;
            switch (severity)
            {
                case Severity::INFO_L:
                    col = GREEN;
                    level = "INFO";
                    break;
                case Severity::DEBUG_L:
                    col = BLUE;
                    level = "DEBUG";
                    break;
                case Severity::TRACE_L:
                    col = RESET_COLOR;
                    level = "TRACE";
                    break;
                case Severity::WARNING_L:
                    col = YELLOW;
                    level = "WARNING";
                    break;
                case Severity::ERROR_L:
                    col = ORANGE;
                    level = "ERROR";
                    break;
                case Severity::CRITICAL_L:
                    col = RED;
                    level = "CRITICAL";
                    break;
                case Severity::NONE_L:
                    col = RESET_COLOR;
                    level = "LOG";
                    break;
            }
            const auto logTime = Time();
            auto logString = col + "[" + logTime + "] " + level + ": " + msg + RESET_COLOR;
                //std::format("{}[{}] {}: {}{}", col, logTime, level, msg, RESET_COLOR);
            std::cout << logString << '\n';
            std::cout.flush();

            if (saveLogs)
            {
                logString = "[" + logTime + "] " + level + ": " + msg + "\n";
                    //std::format("[{}] {}: {}\n", logTime, level, msg);
                logFile.write(logString.c_str(), static_cast<int>(logString.size()));
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
}
