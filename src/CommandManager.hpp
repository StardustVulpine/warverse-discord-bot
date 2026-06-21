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

    // Definition of subcommand structure
    struct SubCommand
    {
        std::string sName;
        std::string sDescription;
        std::vector<dpp::command_option> sOptions;
        CommandCallbackFunction sCallbackFunction;
    };

    // Definition of command structure
    struct Command
    {
        std::string cName;
        std::string cDescription;
        uint64_t requiredPermissions{};
        std::vector<dpp::command_option> cOptions{};
        CommandCallbackFunction cCallbackFunction{};
        std::map<std::string, SubCommand> cSubCommands{};

        Command& AddSubCommand(const std::string& subCommandName,
            const std::string& subCommandDescription, const std::vector<dpp::command_option>& subCommandOptions,
            const CommandCallbackFunction& subCommandCallback)
        {
            cSubCommands[subCommandName] = {
                subCommandName,
                subCommandDescription,
                subCommandOptions,
                subCommandCallback
            };

            return *this;
        }
    };

    class CommandManager
    {
    private:
        // Map linking command name as string to actual command
        std::map<std::string, Command> mCommandsMap;

    public:
        // Adding command to registry
        Command& NewCommand(const std::string& commandName, const std::string& commandDescription, const uint64_t requiredPermissions = 0,
            const std::vector<dpp::command_option>& commandOptions = {}, const CommandCallbackFunction& commandCallback = nullptr)
        {
            mCommandsMap[commandName] = {
                commandName,
                commandDescription,
                requiredPermissions,
                commandOptions,
                commandCallback
            };

            return mCommandsMap[commandName];
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

                    if (command.requiredPermissions > 0)
                    {
                        newCommand.set_default_permissions(command.requiredPermissions);
                        newCommand.set_dm_permission(false);
                    }

                    // Loop through all command's options and add them to command
                    for (auto const& commandParam : command.cOptions) {
                        newCommand.add_option(commandParam);
                    }
                    // Loop through all command's subcommands and add them to command
                    for (auto const& [subCommandNameKey, subCommand] : command.cSubCommands)
                    {
                        // Create new raw subcommand
                        dpp::command_option subOption(dpp::co_sub_command, subCommand.sName, subCommand.sDescription);

                        // Loop through all sub-command's options and add them to sub-command
                        for (auto const& subCommandParam : subCommand.sOptions) {
                            subOption.add_option(subCommandParam);
                        }

                        // Add sub-command to main command
                        newCommand.add_option(subOption);
                    }

                    commandsList.push_back(newCommand); // Push created raw command to vector
                }
#ifdef DEBUG
                botCluster.guild_bulk_command_create(commandsList, TEST_GUILD_ID);
#else
                botCluster.global_bulk_command_create(commandsList);
#endif
            }
        }
        // Handling event when user sends command
        void HandleIncomingCommand(const dpp::slashcommand_t& incomingCommandEvent)
        {
            // Extract command name and search for it in map
            const std::string incomingCommandName = incomingCommandEvent.command.get_command_name();
            if (const auto commandIterator = mCommandsMap.find(incomingCommandName); commandIterator != mCommandsMap.end())
            {
                // Grab subcommands stored as interaction and check if it's not empty or is of type co_sub_command
                dpp::command_interaction interaction = incomingCommandEvent.command.get_command_interaction();
                if (!interaction.options.empty() && interaction.options[0].type == dpp::co_sub_command)
                {
                    // Get subcommand name and search for it in map
                    const std::string subCommandName = interaction.options[0].name;
                    const auto subCommandIterator = commandIterator->second.cSubCommands.find(subCommandName);
                    if (subCommandIterator != commandIterator->second.cSubCommands.end()) {
                        // Run subcommand's lambda function
                        subCommandIterator->second.sCallbackFunction(incomingCommandEvent);
                        return;
                    }
                }

                // If subcommand was not found/triggered, run main command's lambda function if exists
                if (commandIterator->second.cCallbackFunction) {
                    commandIterator->second.cCallbackFunction(incomingCommandEvent);
                }
            }
        }

        const std::map<std::string, Command>& GetAllCommands() const {
            return mCommandsMap;
        }

    };
}
