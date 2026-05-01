//
// Created by stardustvulpine on 4/8/26.
//

#include "Bot.hpp"


namespace wdb
{
    constexpr dpp::snowflake TEST_GUILD_ID = 1486723392718639156;

    void Bot::UpdateCommands()
    {
        Log::Trace("{} {}", __func__, " updating bot events...");

        /* Add new command template:
        mCommandManager.Add(
        "",
        "",
        {},
        [this](const dpp::slashcommand_t& event)
        {

        });
        */

        mCommandManager.Add(
        "ping",
        "Ping Pong",
        {},
        [](const dpp::slashcommand_t& event)
        {
            std::string issuingUsername = event.command.get_issuing_user().username;
            Log::Print("{} used command: {}", issuingUsername, event.command.get_command_name());

            dpp::snowflake userID = event.command.get_issuing_user().id;
            event.reply(std::format("Pong dong! <@{}>", std::to_string(userID)));
        });

        mCommandManager.Add(
        "dm",
        "Sends a direct message to a user",
        {
            dpp::command_option(dpp::co_mentionable, "user", "The user to message", false),
            dpp::command_option(dpp::co_string, "message", "The message to send", false)
        },
        [this](const dpp::slashcommand_t& event)
        {
            std::string issuingUsername = event.command.get_issuing_user().username;
                Log::Print("{} used command: {}", issuingUsername, event.command.get_command_name());

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
                mBotCluster.direct_message_create(user, dpp::message(message), [event, user, issuingUsername](const dpp::confirmation_callback_t& callback){
                    /* If the callback errors, we want to send a message telling the author that something went wrong. */
                    if (callback.is_error()) {
                        /* Here, we want the error message to be different if the user we're trying to send a message to is the command author. */
                        if (user == event.command.get_issuing_user().id) {
                            event.reply(dpp::message("I couldn't send you a message.").set_flags(dpp::m_ephemeral));
                        } else {
                            event.reply(dpp::message("I couldn't send a message to that user. Please check that is a valid user!").set_flags(dpp::m_ephemeral));
                        }
                        Log::Print("{} used command: {}. Message could not be delivered.", issuingUsername, event.command.get_command_name());
                        return;
                    }

                    /* We do the same here, so the message is different if it's to the command author or if it's to a specified user. */
                    if (user == event.command.get_issuing_user().id) {
                        event.reply(dpp::message("I've sent you a private message.")/*.set_flags(dpp::m_ephemeral)*/);
                        Log::Print("{} used command: {}. Message was sent.", issuingUsername, event.command.get_command_name());
                    } else {
                        event.reply(dpp::message("I've sent a message to that user.")/*.set_flags(dpp::m_ephemeral)*/);
                    }
                });
        });

        mCommandManager.Add(
        "add_user",
        "Add user to database",
        {dpp::command_option(dpp::co_mentionable, "user", "The user to add", true)},
        [this](const dpp::slashcommand_t& event)
        {
            if (event.get_parameter("user").index() == 0) {
                    event.reply(dpp::message("Couldn't add user. No user provided.").set_flags(dpp::m_ephemeral));
                    return;
                }
                dpp::user user = event.command.get_resolved_user(
                    std::get<dpp::snowflake>(event.get_parameter("user")));

                try
                {
                    m_dbManager->AddNewUser(user.username, user.id);
                    const std::string reply = std::format(R"(User '{}' with ID '{}' has been added to database.)",
                        user.username, std::to_string(user.id));
                    event.reply(dpp::message(reply));
                }
                catch (std::exception &e)
                {
                    Log::Error(e.what());
                    if (!std::string(e.what()).compare("UNIQUE constraint failed: Users.DiscordID"))
                    {
                        const std::string reply = std::format(R"(User '{}' with ID '{}' already exists in database.)",
                            user.username, std::to_string(user.id));
                        event.reply(dpp::message(reply));
                    }
                }
        });

        mCommandManager.Add(
        "show_users",
        "List all users in database",
        {},
        [this](const dpp::slashcommand_t& event)
        {
            event.reply(dpp::message(m_dbManager->GetAllUsers()));
        });

        mCommandManager.Add(
        "show_fractions",
        "List all fractions in database",
        {},
        [this](const dpp::slashcommand_t& event)
        {
            event.reply(dpp::message(m_dbManager->GetAllFractions()));
        });

        // Listen for leveling bot messages and catch mentioned user.
        mBotCluster.on_message_create([](const dpp::message_create_t& event) {
            constexpr dpp::snowflake targetChannelID = 1491554468821602377;
            constexpr dpp::snowflake targetUserID = 336562353795760131;

            // Check if sent in correct channel and by correct user and contains any mentions
            if (event.msg.channel_id != targetChannelID) { return; }
            if (event.msg.author.id != targetUserID) { return; }
            if (event.msg.mentions.empty()) { return; }

            dpp::user mentionedUser = event.msg.mentions[0].first;

            const std::string replyMessage = std::format("Detected that user <@{}> mentioned <@{}>!", std::to_string(event.msg.author.id), std::to_string(mentionedUser.id));

            Log::Info("Detected that user <@{}> mentioned <@{}>!", event.msg.author.username, mentionedUser.username);

            event.reply(replyMessage);
        });
    }

    void Bot::Run()
    {
        UpdateCommands();
        Log::Info("Bot started!");
        mBotCluster.start(dpp::st_wait);
    }
} // warverse