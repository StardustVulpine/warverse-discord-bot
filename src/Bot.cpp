//
// Created by stardustvulpine on 4/8/26.
//

#include "Bot.hpp"
#include <Log.hpp>



namespace wdb {
    constexpr dpp::snowflake TEST_GUILD_ID = 1486723392718639156;

    void Bot::UpdateCommands()
    {
        mBotCluster.on_slashcommand([](const dpp::slashcommand_t& event)
        {
            if (event.command.get_command_name() == "dupa") {
                event.reply("Dupa!");
            }
        });

        mBotCluster.on_slashcommand([](const dpp::slashcommand_t& event)
        {
            if (event.command.get_command_name() == "ping") {
                dpp::snowflake userID = event.command.get_issuing_user().id;
                event.reply(std::format("Pong! <@{}>", std::to_string(userID)));
            }
        });

        mBotCluster.on_slashcommand([this](const dpp::slashcommand_t& event) {
            if (event.command.get_command_name() == "pm") {
                dpp::snowflake user;
                std::string message;

                /* If there was no specified user, we set the "user" variable to the command author (issuing user). */
                if (event.get_parameter("user").index() == 0) {
                    user = event.command.get_issuing_user().id;
                } else { /* Otherwise, we set it to the specified user! */
                    user = std::get<dpp::snowflake>(event.get_parameter("user"));
                }

                /* If there was no specified message, we set a default one. index() == 0 means the parameter is empty (std::monostate) */
                if (event.get_parameter("message").index() == 0) {
                    message = "Here's a private message!";
                } else { /* Otherwise, we set it to the specified message! */
                    message = std::get<std::string>(event.get_parameter("message"));
                }

                /* Send a message to the user set above. */
                mBotCluster.direct_message_create(user, dpp::message(message), [event, user](const dpp::confirmation_callback_t& callback){
                    /* If the callback errors, we want to send a message telling the author that something went wrong. */
                    if (callback.is_error()) {
                        /* Here, we want the error message to be different if the user we're trying to send a message to is the command author. */
                        if (user == event.command.get_issuing_user().id) {
                            event.reply(dpp::message("I couldn't send you a message.").set_flags(dpp::m_ephemeral));
                        } else {
                            event.reply(dpp::message("I couldn't send a message to that user. Please check that is a valid user!").set_flags(dpp::m_ephemeral));
                        }

                        return;
                    }

                    /* We do the same here, so the message is different if it's to the command author or if it's to a specified user. */
                    if (user == event.command.get_issuing_user().id) {
                        event.reply(dpp::message("I've sent you a private message.")/*.set_flags(dpp::m_ephemeral)*/);
                    } else {
                        event.reply(dpp::message("I've sent a message to that user.")/*.set_flags(dpp::m_ephemeral)*/);
                    }
                });
            }
        });
    }

    void Bot::RegisterCommands()
    {
        mBotCluster.on_ready([this](const dpp::ready_t&) {
            if (dpp::run_once<struct register_guild_commands>())
            {
                std::vector<dpp::slashcommand> Commands;

                Commands.emplace_back("ping", "Ping pong!", mBotCluster.me.id);

                auto& pm = Commands.emplace_back("pm", "Send a private message.", mBotCluster.me.id);
                pm.add_option(dpp::command_option(dpp::co_mentionable, "user", "The user to message", false));
                pm.add_option(dpp::command_option(dpp::co_string, "message", "The message to send", false));

                Commands.emplace_back("dupa", "dupa dupa!", mBotCluster.me.id);

                mBotCluster.guild_bulk_command_create(Commands, TEST_GUILD_ID);
                mBotCluster.global_bulk_command_delete();
            }
        });
    }

    void Bot::Start()
    {
        UpdateCommands();
        Log::Trace("Updating commands...");
        RegisterCommands();
        Log::Trace("Commands updated and registered!");
        Log::Info("Bot started!");
        mBotCluster.start(dpp::st_wait);
    }
} // warverse