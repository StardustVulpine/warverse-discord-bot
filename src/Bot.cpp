//
// Created by stardustvulpine on 4/8/26.
//

#include "Bot.hpp"


namespace wdb
{
    void Bot::Commands()
    {
        Log::Trace("{} {}", __func__, " updating bot events...");

        /* Add new command template:
        mCommandManager.NewCommand(
        "",
        "",
        {},
        [this](const dpp::slashcommand_t& event)
        {

        });
        */

        mCommandManager.NewCommand(
        "help",
        "Shows list of all available commands",
        0,
        {},
        [this](const dpp::slashcommand_t& event)
        {
            dpp::embed embed;
            embed.set_color(dpp::colors::blurple);
            embed.set_title("Bot Commands List");
            embed.set_description("Here is list of all available commands:");

            // Get all bot commands and for create field for each
            const auto& allBotCommands = this->mCommandManager.GetAllCommands();
            for (const auto& [commandName, command] : allBotCommands)
            {
                std::string fieldContent;
                if (!command.cDescription.empty()) {
                    fieldContent = "_"+command.cDescription+"_";
                }

                // If command has subcommands, add them to field
                if (!command.cSubCommands.empty())
                {
                    fieldContent += "\n**Subcommands:**\n";
                    for (const auto& [subCommandName, subCommand] : command.cSubCommands) {
                        fieldContent += "- `/"+command.cName+" "+subCommand.sName+"` _"+subCommand.sDescription+"_\n";
                    }
                }
                embed.add_field("`/"+command.cName+"` ", fieldContent, false);
            }
            dpp::message replyMsg;
            replyMsg.add_embed(embed);
            event.reply(replyMsg);
        });

        mCommandManager.NewCommand(
        "ping",
        "Test bot connection",
        0,
        {},
        [](const dpp::slashcommand_t& event)
        {
            std::string issuingUsername = event.command.get_issuing_user().username;
            Log::Print("{} used command: {}", issuingUsername, event.command.get_command_name());

            dpp::snowflake userID = event.command.get_issuing_user().id;
            event.reply(std::format("Pong <@{}>!", std::to_string(userID)));
        });

        mCommandManager.NewCommand(
        "dm",
        "Sends a direct message to a user",
        0,
        {
            dpp::command_option(dpp::co_mentionable, "user", "The user to message", false),
            dpp::command_option(dpp::co_string, "message", "The message to send", false)},
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

        mCommandManager.NewCommand(
        "add",
        "Required permissions: Manage Guild",
        dpp::p_manage_guild,
        {},
        nullptr
        );
        mCommandManager.AddSubCommand(
            "add",
            "user",
            "Add new user to database",
            {dpp::command_option(dpp::co_mentionable, "user", "The user to add", true)},
            [this](const dpp::slashcommand_t& event)
            {
                if (event.get_parameter("user").index() == 0) {
                    event.reply(dpp::message("Couldn't add user. No user provided.").set_flags(dpp::m_ephemeral));
                    return;
                }
                dpp::user user = event.command.get_resolved_user(std::get<dpp::snowflake>(event.get_parameter("user")));

                try {
                    m_dbManager->AddNewUser(user.username, user.id);

                    dpp::message replyMsg;
                    replyMsg.add_embed(dpp::embed()
                        .set_title(":white_check_mark: User added to database!")
                        .set_color(dpp::colors::green)
                        .set_description(std::format("**User:** <@{}>\n**ID:** `{}`",
                            std::to_string(user.id), std::to_string(user.id)))
                        .set_thumbnail(user.get_avatar_url()));
                    event.reply(replyMsg);
                }
                catch (SQLite::Exception &e) {
                    Log::Error("SQLite Error " + std::to_string(e.getErrorCode()) + ": " + e.what());
                    if (e.getErrorCode() == 19) // SQLITE UNIQUE constraint failed error
                    {
                        const std::string reply = std::format(R"(User '{}' with ID '{}' already exists in database.)",
                            user.username, std::to_string(user.id));
                        event.reply(dpp::message(reply));
                    }
                }
            }
        );
        mCommandManager.AddSubCommand(
            "add",
            "fraction",
            "Add new fraction to database",
            {
                dpp::command_option(dpp::co_string, "name", "The name of the fraction", true),
                dpp::command_option(dpp::co_string, "description", "The description of the fraction", true),
                dpp::command_option(dpp::co_role, "fraction_role", "The role of the fraction", true)
            },
            [this](const dpp::slashcommand_t& event)
            {
                if (event.get_parameter("name").index() == 0 ||
                event.get_parameter("description").index() == 0 ||
                event.get_parameter("fraction_role").index() == 0)
                {
                    event.reply(dpp::message("Couldn't add fraction. Not enough parameters provided").set_flags(dpp::m_ephemeral));
                    return;
                }
                const std::string name = std::get<std::string>(event.get_parameter("name"));
                const std::string description = std::get<std::string>(event.get_parameter("description"));
                dpp::role fractionRole = event.command.get_resolved_role(std::get<dpp::snowflake>(event.get_parameter("fraction_role")));

                try {
                    m_dbManager->AddNewFraction(name, description, fractionRole.id);
                    const std::string reply = std::format(R"(Fraction '{}' with assigned role <@&{}> has been added to database.)",
                        name, std::to_string(fractionRole.id));
                    event.reply(dpp::message(reply));
                }
                catch (std::exception &e) {
                    Log::Error(e.what());
                    if (std::string(e.what()) == "UNIQUE constraint failed: Fractions.DiscordRoleID")
                    {
                        const std::string reply = std::format(R"(Fraction with assigned role <@&{}> already exists in database.)",
                            std::to_string(fractionRole.id));
                        event.reply(dpp::message(reply));
                    }
                }
            }
        );

        mCommandManager.NewCommand(
        "show",
        "",
        0,
        {},
        nullptr
        );
        mCommandManager.AddSubCommand(
            "show",
            "users",
            "List all users in database",
            {},
            [this](const dpp::slashcommand_t& event)
            {
                dpp::message replyMsg;
                json allUsers = m_dbManager->GetAllUsers();
                const dpp::embed embed(&allUsers);
                replyMsg.add_embed(embed);

                event.reply(replyMsg);
            }
        );
        mCommandManager.AddSubCommand(
            "show",
            "fractions",
            "List all fractions in database",
            {},
            [this](const dpp::slashcommand_t& event)
            {
                event.reply(dpp::message(m_dbManager->GetAllFractions()));
            }
        );

        mCommandManager.NewCommand(
        "button",
        "test of a button",
        0,
        {},
        [this](const dpp::slashcommand_t& event)
        {
            dpp::message msg(event.command.channel_id, "this text has a button");

            msg.add_component(
                dpp::component().add_component(
                    dpp::component()
                    .set_label("Click Me!")
                    .set_type(dpp::cot_button)
                    .set_style(dpp::cos_primary)
                    .set_id("testButton")
                )
            );
            event.reply(msg);
        });

        mBotCluster.on_button_click([this](const dpp::button_click_t& event){
            event.reply("You clicked: " + event.custom_id);
        });

    }

    void Bot::MessageListeners()
    {
        // Listen for leveling bot messages and catch mentioned user.
        mBotCluster.on_message_create([](const dpp::message_create_t& event)
        {
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
        Commands();
        MessageListeners();
        Log::Info("Bot started!");
        mBotCluster.start(dpp::st_wait);
    }
}