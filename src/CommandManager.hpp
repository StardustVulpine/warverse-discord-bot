//
// Created by stardustvulpine on 4/28/26.
//

#pragma once

#include <dpp/dpp.h>
#include <functional>
#include <string>
#include <map>
#include <vector>

namespace wdb::commands
{
    constexpr dpp::snowflake TEST_GUILD_ID = 1486723392718639156;

    // Definition of lambda function structure that takes dpp::slashcommand_t as parameter and returns nothing
    using CommandCallbackFunction = std::function<void(const dpp::slashcommand_t&)>;

    // Definition of command structure
    struct Command
    {
        std::string cName;
        std::string cDescription;
        std::vector<dpp::command_option> cOptions;
        CommandCallbackFunction cCallbackAction;

    };

    class CommandManager
    {
    private:
        // Map linking command name as string to actual command
        std::map<std::string, Command> mCommandsMap;

    public:
        // Adding command to memory
        void Add(const std::string& name, const std::string& description,
            const std::vector<dpp::command_option>& options, const CommandCallbackFunction& callback)
        {
            mCommandsMap[name] = {
                name,
                description,
                options,
                callback
            };
        }

        // Registering all mapped commands to Discord
        void RegisterCommands(dpp::cluster& botCluster)
        {
            if (dpp::run_once<struct register_guild_commands>())
            {
                std::vector<dpp::slashcommand> commandsList;

                // Loop through every command
                for (auto const& [nameKey, command] : mCommandsMap)
                {
                    // Create new raw command
                    dpp::slashcommand newCommand(
                        command.cName,
                        command.cDescription,
                        botCluster.me.id
                    );

                    // Loop through all command's options and add them to command
                    for (auto const& commandParam : command.cOptions) {
                        newCommand.add_option(commandParam);
                    }

                    commandsList.push_back(newCommand); // Push created raw command to vector
                }
                botCluster.guild_bulk_command_create(commandsList, TEST_GUILD_ID);
            }
        }
        // Handling event when user sends command
        void HandleIncomingCommand(const dpp::slashcommand_t& incomingCommandEvent)
        {
            // Extract command name and search for it in map
            std::string incomingCommandName = incomingCommandEvent.command.get_command_name();
            auto commandIterator = mCommandsMap.find(incomingCommandName);

            // If found in map, grab lambda associated with command and execute it
            if (commandIterator != mCommandsMap.end()) {
                commandIterator->second.cCallbackAction(incomingCommandEvent);
            }
        }

    };
}
