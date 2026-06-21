//
// Created by stardustvulpine on 4/8/26.
//

#include "Bot.hpp"
#include <cpr/cpr.h>

namespace wdb
{
    const std::string PLACEHOLDER_FRACTION_IMAGE = "https://cdn.discordapp.com/attachments/1518336173309820998/1518342895956922589/clan.png?ex=6a399283&is=6a384103&hm=ddbf8027c11cf48ae2d42d56ab588c95a67f33d47bede3aec5ef61ab0c366a01";

    void Bot::OnCommandSentEvent()
    {
//region GENERAL COMMANDS
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

//endregion
//region DB MANAGEMENT COMMANDS

    //region CMD ADD
        mCommandManager.NewCommand(
        "register",
        "Required permissions: Manage Guild",
        dpp::p_manage_guild)
        .AddSubCommand(
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
                const dpp::guild_member member = event.command.get_resolved_member(std::get<dpp::snowflake>(event.get_parameter("user")));

                std::string userID = std::to_string(user.id);
                std::string userAvatarURL = member.get_avatar_url();
                if (userAvatarURL.empty()) {
                    userAvatarURL = user.get_avatar_url();
                }

                if (!std::filesystem::exists(Common::GetImagesDir())) {
                    std::filesystem::create_directories(Common::GetImagesDir());
                }

                // To Do Later: Caching user's avatars along with DB path entry, and sending them as embed thumbnails instead of relaying on discord' URL's

                /*std::string imgFilepath = std::format("{}/{}.png", Common::GetImagesDir(), userID);
                std::ofstream file(imgFilepath, std::ios::binary);
                cpr::Response r = cpr::Download(file, cpr::Url{userAvatarURL});
                if (r.status_code == 200) {
                    Log::Info("Downloaded image: {}", r.text);
                } else {
                    Log::Error("Download failed: {}", r.status_code);
                }

                Log::Debug("User Avatar URL: " + userAvatarURL);*/

                try {
                    m_dbManager->AddNewUser(user.username, user.id);

                    dpp::message replyMsg;
                    replyMsg.add_embed(dpp::embed()
                        .set_title(":white_check_mark: User added to database!")
                        .set_color(dpp::colors::green)
                        .set_description(std::format("**User:** <@{}>\n**ID:** `{}`", userID, userID))
                        .set_thumbnail(userAvatarURL));
                    event.reply(replyMsg);
                }
                catch (SQLite::Exception &e) {
                    Log::Error("SQLite Error (" + std::to_string(e.getErrorCode()) + ") : " + e.what());
                    if (e.getErrorCode() == db::error_code::SQLITE_UNIQUE_CONSTRAINT)
                    {
                        dpp::message replyMsg;
                        replyMsg.add_embed(dpp::embed()
                            .set_title(":x: User already exists in database!")
                            .set_color(dpp::colors::red)
                            .set_description(std::format("**User:** <@{}>\n**ID:** `{}`", userID, userID))
                            .set_thumbnail(userAvatarURL));
                        event.reply(replyMsg);
                    }
                }
            }
        )
        /*.AddSubCommand(
            "all_users",
            "Bulk registration of all guild members",
            {},
            [this](const dpp::slashcommand_t& event)
            {
                // Not implemented yet
            }
        )*/
        .AddSubCommand(
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
                catch (SQLite::Exception &e) {
                    Log::Error("SQLite Error (" + std::to_string(e.getErrorCode()) + ") : " + e.what());
                    if (e.getErrorCode() == db::error_code::SQLITE_UNIQUE_CONSTRAINT)
                    {
                        const std::string reply = std::format(R"(Fraction with assigned role <@&{}> already exists in database.)",
                            std::to_string(fractionRole.id));
                        event.reply(dpp::message(reply));
                    }
                }
            }
        );

    //endregion
    //region CMD REMOVE

        commands::Command& removeCMD =  mCommandManager.NewCommand(
            "remove",
            "Removes users/fractions from database.\nRequired permissions: Manage Guild",
            dpp::p_manage_guild
        )
        .AddSubCommand(
            "user",
            "Remove user from database",
            {dpp::command_option(dpp::co_mentionable, "user", "User to be removed", true)},
            [this](const dpp::slashcommand_t& event)
            {
                if (event.get_parameter("user").index() == 0) {
                    event.reply(dpp::message("Couldn't remove user. No user provided.").set_flags(dpp::m_ephemeral));
                    return;
                }
                dpp::user user = event.command.get_resolved_user(std::get<dpp::snowflake>(event.get_parameter("user")));

                try {
                    if (!m_dbManager->UserExists(user.id)) {
                        event.reply(dpp::message("User not found.").set_flags(dpp::m_ephemeral));
                        return;
                    }
                } catch (SQLite::Exception &e) {
                    Log::Error("SQLite Error (" + std::to_string(e.getErrorCode()) + ") : " + e.what());
                    event.reply(dpp::message("SQLite Database Error (" + std::to_string(e.getErrorCode()) + ") : " + e.what()));
                    return;
                }


                const dpp::guild_member member = event.command.get_resolved_member(std::get<dpp::snowflake>(event.get_parameter("user")));

                std::string userID = std::to_string(user.id);
                std::string userAvatarURL = member.get_avatar_url();
                if (userAvatarURL.empty()) {
                    userAvatarURL = user.get_avatar_url();
                }

                const dpp::embed embed = dpp::embed()
                    .set_title(":wastebasket: Are you sure to delete this user from database?")
                    .set_color(dpp::colors::red_blood)
                    .set_description(std::format("**User:** <@{}>\n**ID:** `{}`", userID, userID))
                    .set_thumbnail(userAvatarURL);

                const std::string yesButtonID = "REMOVE_USER_CONFIRM_BUTTON-YES:" + userID;
                const std::string noButtonID = "REMOVE_USER_CONFIRM_BUTTON-NO";
                const dpp::component yesButton = dpp::component()
                    .set_label("Yes")
                    .set_type(dpp::cot_button)
                    .set_style(dpp::cos_danger)
                    .set_id(yesButtonID);
                const dpp::component noButton = dpp::component()
                    .set_label("No")
                    .set_type(dpp::cot_button)
                    .set_style(dpp::cos_primary)
                    .set_id(noButtonID);
                auto buttons = dpp::component();
                buttons.add_component(yesButton);
                buttons.add_component(noButton);

                dpp::message replyMsg;
                replyMsg.add_embed(embed);
                replyMsg.add_component(buttons);
                event.reply(replyMsg);
            });
            mBotCluster.on_button_click([this](const dpp::button_click_t& event){
                // Disable buttons when any of them was clicked
                if (event.custom_id.find("REMOVE_USER_CONFIRM_BUTTON") == 0) {
                    dpp::message updatedMsg = event.command.msg;
                    if (!updatedMsg.components.empty()) {
                        for (auto& component : updatedMsg.components[0].components) {
                            component.set_disabled(true);
                        }
                    }
                    event.reply(dpp::ir_update_message, updatedMsg);
                }
                if (event.custom_id.find("REMOVE_USER_CONFIRM_BUTTON-YES:") == 0)
                {
                    const std::string targetID_str = event.custom_id.substr(31);
                    const DiscordID targetID = std::stoull(targetID_str);

                    try {
                        m_dbManager->RemoveUser(targetID);
                        dpp::message followUpMsg(event.command.channel_id, ":wastebasket: User removed from database!");
                        followUpMsg.set_reference(event.command.msg.id);

                        mBotCluster.message_create(followUpMsg);
                    }
                    catch (SQLite::Exception &e) {
                        Log::Error("SQLite Error (" + std::to_string(e.getErrorCode()) + ") : " + e.what());
                        dpp::message followUpMsg(event.command.channel_id, "SQLite Database Error (" + std::to_string(e.getErrorCode()) + ") : " + e.what());
                        followUpMsg.set_reference(event.command.msg.id);

                        mBotCluster.message_create(followUpMsg);
                    }
                    catch (std::exception& e) {
                        Log::Error(e.what());

                        dpp::message followUpMsg(event.command.channel_id, "There was error. Check bot logs for more info.");
                        followUpMsg.set_reference(event.command.msg.id);

                        mBotCluster.message_create(followUpMsg);
                    }
                }
                if (event.custom_id == "REMOVE_USER_CONFIRM_BUTTON-NO") {
                    dpp::message followUpMsg(event.command.channel_id, ":x: Operation cancelled.");
                    followUpMsg.set_reference(event.command.msg.id);

                    mBotCluster.message_create(followUpMsg);
                }
            }
        );
        removeCMD.AddSubCommand(
            "fraction",
            "Remove fraction from database",
            {dpp::command_option(dpp::co_role, "fraction_role", "Role associated with fraction you want to remove", true)},
            [this](const dpp::slashcommand_t& event)
            {
                if (event.get_parameter("fraction_role").index() == 0) {
                    event.reply(dpp::message("Couldn't remove fraction without specifying one.").set_flags(dpp::m_ephemeral));
                    return;
                }
                const DiscordID fractionID = event.command.get_resolved_role(std::get<dpp::snowflake>(event.get_parameter("fraction_role"))).id;


                try {
                    if (!m_dbManager->FractionExists(fractionID)) {
                        event.reply(dpp::message("Fraction not found.").set_flags(dpp::m_ephemeral));
                        return;
                    }

                    std::string fractionName = m_dbManager->GetFractionNameByID(fractionID);

                    const dpp::embed embed = dpp::embed()
                        .set_title(":wastebasket: Are you sure to delete this fraction from database?")
                        .set_color(dpp::colors::red_blood)
                        .set_description(std::format("**Name:** {}\n**Role:** <@{}>", fractionName, std::to_string(fractionID)));

                    const std::string yesButtonID = "REMOVE_FRACTION_CONFIRM_BUTTON-YES:" + std::to_string(fractionID);
                    const std::string noButtonID = "REMOVE_FRACTION_CONFIRM_BUTTON-NO";

                    const dpp::component yesButton = dpp::component()
                        .set_label("Yes")
                        .set_type(dpp::cot_button)
                        .set_style(dpp::cos_danger)
                        .set_id(yesButtonID);
                    const dpp::component noButton = dpp::component()
                        .set_label("No")
                        .set_type(dpp::cot_button)
                        .set_style(dpp::cos_primary)
                        .set_id(noButtonID);
                    auto buttons = dpp::component();
                    buttons.add_component(yesButton);
                    buttons.add_component(noButton);

                    dpp::message replyMsg;
                    replyMsg.add_embed(embed);
                    replyMsg.add_component(buttons);
                    event.reply(replyMsg);
                }
                catch (SQLite::Exception &e) {
                    Log::Error("SQLite Error (" + std::to_string(e.getErrorCode()) + ") : " + e.what());
                    event.reply(dpp::message("SQLite Database Error (" + std::to_string(e.getErrorCode()) + ") : " + e.what()));
                }
            });
            mBotCluster.on_button_click([this](const dpp::button_click_t& event){
                // Disable buttons when any of them was clicked
                if (event.custom_id.find("REMOVE_FRACTION_CONFIRM_BUTTON") == 0) {
                    dpp::message updatedMsg = event.command.msg;
                    if (!updatedMsg.components.empty()) {
                        for (auto& component : updatedMsg.components[0].components) {
                            component.set_disabled(true);
                        }
                    }
                    event.reply(dpp::ir_update_message, updatedMsg);
                }
                if (event.custom_id.find("REMOVE_FRACTION_CONFIRM_BUTTON-YES:") == 0)
                {
                    const std::string targetID_str = event.custom_id.substr(35);
                    const DiscordID targetID = std::stoull(targetID_str);

                    try {
                        m_dbManager->RemoveFraction(targetID);

                        dpp::message followUpMsg(event.command.channel_id, ":wastebasket: Fraction removed from database!");
                        followUpMsg.set_reference(event.command.msg.id);

                        mBotCluster.message_create(followUpMsg);
                    }
                    catch (SQLite::Exception &e) {
                        Log::Error("SQLite Error (" + std::to_string(e.getErrorCode()) + ") : " + e.what());
                        dpp::message followUpMsg(event.command.channel_id, "SQLite Database Error (" + std::to_string(e.getErrorCode()) + ") : " + e.what());
                        followUpMsg.set_reference(event.command.msg.id);

                        mBotCluster.message_create(followUpMsg);
                    }
                    catch (std::exception& e) {
                        Log::Error(e.what());

                        dpp::message followUpMsg(event.command.channel_id, "There was error. Check bot logs for more info.");
                        followUpMsg.set_reference(event.command.msg.id);

                        mBotCluster.message_create(followUpMsg);
                    }
                }
                if (event.custom_id == "REMOVE_FRACTION_CONFIRM_BUTTON-NO") {
                    dpp::message followUpMsg(event.command.channel_id, ":x: Operation cancelled.");
                    followUpMsg.set_reference(event.command.msg.id);

                    mBotCluster.message_create(followUpMsg);
                }
            });
        removeCMD.AddSubCommand(
            "all_users",
            "Remove ALL USERS from database (USE WITH CAUTION!)",
            {},
            [this](const dpp::slashcommand_t& event)
            {
                const dpp::embed embed = dpp::embed()
                        .set_title(":wastebasket: Are you sure to delete ALL USERS from database?")
                        .set_color(dpp::colors::red_blood)
                        .set_description(":warning: ***THIS ACTION CANNOT BE UNDONE*** :warning:");

                const std::string yesButtonID = "REMOVE_ALL_USERS_CONFIRM_BUTTON-YES";
                const std::string noButtonID = "REMOVE_ALL_USERS_CONFIRM_BUTTON-NO";
                const dpp::component yesButton = dpp::component()
                    .set_label("Yes")
                    .set_type(dpp::cot_button)
                    .set_style(dpp::cos_danger)
                    .set_id(yesButtonID);
                const dpp::component noButton = dpp::component()
                    .set_label("No")
                    .set_type(dpp::cot_button)
                    .set_style(dpp::cos_primary)
                    .set_id(noButtonID);
                auto buttons = dpp::component();
                buttons.add_component(yesButton);
                buttons.add_component(noButton);

                dpp::message replyMsg;
                replyMsg.add_embed(embed);
                replyMsg.add_component(buttons);
                event.reply(replyMsg);
            });
            mBotCluster.on_button_click([this](const dpp::button_click_t& event){
                // Disable buttons when any of them was clicked
                if (event.custom_id.find("REMOVE_ALL_USERS_CONFIRM_BUTTON") == 0) {
                    dpp::message updatedMsg = event.command.msg;
                    if (!updatedMsg.components.empty()) {
                        for (auto& component : updatedMsg.components[0].components) {
                            component.set_disabled(true);
                        }
                    }
                    event.reply(dpp::ir_update_message, updatedMsg);
                }
                if (event.custom_id == "REMOVE_ALL_USERS_CONFIRM_BUTTON-YES")
                {
                    try {
                        m_dbManager->RemoveAllUsers();
                        dpp::message followUpMsg(event.command.channel_id, ":wastebasket: ALL USERS have been REMOVED from database.");
                        followUpMsg.set_reference(event.command.msg.id);

                        mBotCluster.message_create(followUpMsg);
                    }
                    catch (SQLite::Exception &e) {
                        Log::Error("SQLite Error (" + std::to_string(e.getErrorCode()) + ") : " + e.what());
                        dpp::message followUpMsg(event.command.channel_id, "SQLite Database Error (" + std::to_string(e.getErrorCode()) + ") : " + e.what());
                        followUpMsg.set_reference(event.command.msg.id);

                        mBotCluster.message_create(followUpMsg);
                    }
                    catch (std::exception& e) {
                        Log::Error(e.what());

                        dpp::message followUpMsg(event.command.channel_id, "There was error. Check bot logs for more info.");
                        followUpMsg.set_reference(event.command.msg.id);

                        mBotCluster.message_create(followUpMsg);
                    }
                }
                if (event.custom_id == "REMOVE_ALL_USERS_CONFIRM_BUTTON-NO") {
                    dpp::message followUpMsg(event.command.channel_id, ":x: Operation cancelled.");
                    followUpMsg.set_reference(event.command.msg.id);

                    mBotCluster.message_create(followUpMsg);
                }
            });
        removeCMD.AddSubCommand(
            "all_fractions",
            "Remove ALL FRACTIONS from database (USE WITH CAUTION!)",
            {},
            [this](const dpp::slashcommand_t& event)
            {
                const dpp::embed embed = dpp::embed()
                    .set_title(":wastebasket: Are you sure to delete ALL FRACTIONS from database?")
                    .set_color(dpp::colors::red_blood)
                    .set_description(":warning: ***THIS ACTION CANNOT BE UNDONE*** :warning:");

                const std::string yesButtonID = "REMOVE_ALL_FRACTIONS_CONFIRM_BUTTON-YES";
                const std::string noButtonID = "REMOVE_ALL_FRACTIONS_CONFIRM_BUTTON-NO";

                const dpp::component yesButton = dpp::component()
                    .set_label("Yes")
                    .set_type(dpp::cot_button)
                    .set_style(dpp::cos_danger)
                    .set_id(yesButtonID);
                const dpp::component noButton = dpp::component()
                    .set_label("No")
                    .set_type(dpp::cot_button)
                    .set_style(dpp::cos_primary)
                    .set_id(noButtonID);
                auto buttons = dpp::component();
                buttons.add_component(yesButton);
                buttons.add_component(noButton);

                dpp::message replyMsg;
                replyMsg.add_embed(embed);
                replyMsg.add_component(buttons);
                event.reply(replyMsg);
            });
            mBotCluster.on_button_click([this](const dpp::button_click_t& event){
                // Disable buttons when any of them was clicked
                if (event.custom_id.find("REMOVE_ALL_FRACTIONS_CONFIRM_BUTTON") == 0) {
                    dpp::message updatedMsg = event.command.msg;
                    if (!updatedMsg.components.empty()) {
                        for (auto& component : updatedMsg.components[0].components) {
                            component.set_disabled(true);
                        }
                    }
                    event.reply(dpp::ir_update_message, updatedMsg);
                }
                if (event.custom_id == "REMOVE_ALL_FRACTIONS_CONFIRM_BUTTON-YES")
                {
                    try {
                        m_dbManager->RemoveAllFractions();

                        dpp::message followUpMsg(event.command.channel_id, ":wastebasket: ALL FRACTIONS have been REMOVED from database.");
                        followUpMsg.set_reference(event.command.msg.id);

                        mBotCluster.message_create(followUpMsg);
                    }
                    catch (SQLite::Exception &e) {
                        Log::Error("SQLite Error (" + std::to_string(e.getErrorCode()) + ") : " + e.what());
                        dpp::message followUpMsg(event.command.channel_id, "SQLite Database Error (" + std::to_string(e.getErrorCode()) + ") : " + e.what());
                        followUpMsg.set_reference(event.command.msg.id);

                        mBotCluster.message_create(followUpMsg);
                    }
                    catch (std::exception& e) {
                        Log::Error(e.what());

                        dpp::message followUpMsg(event.command.channel_id, "There was error. Check bot logs for more info.");
                        followUpMsg.set_reference(event.command.msg.id);

                        mBotCluster.message_create(followUpMsg);
                    }
                }
                if (event.custom_id == "REMOVE_ALL_FRACTIONS_CONFIRM_BUTTON-NO") {
                    dpp::message followUpMsg(event.command.channel_id, ":x: Operation cancelled.");
                    followUpMsg.set_reference(event.command.msg.id);

                    mBotCluster.message_create(followUpMsg);
                }
            });

    //endregion
    //region DB SHOW

        mCommandManager.NewCommand(
        "show",
        "Shows users/fractions in database"
        )
        .AddSubCommand(
            "users",
            "List all users in database",
            {},
            [this](const dpp::slashcommand_t& event)
            {
                json allUsers;
                try {
                    allUsers = m_dbManager->GetAllUsers();
                } catch (SQLite::Exception &e) {
                    Log::Error("SQLite Error (" + std::to_string(e.getErrorCode()) + ") : " + e.what());
                }

                std::string table = std::string()
                    .append("```\n")
                    .append("|       User       |         ID          |    Fraction    |\n")
                    .append("|------------------|---------------------|----------------|\n");

                for (const auto& user : allUsers)
                {
                    std::string username = user.value("discordUsername", "none");
                    std::string discord_id = user.value("discordID", "none");
                    std::string fraction = user.value("fraction", "none");

                    username = stardustvulpine::Utils::PadString(username, 16);
                    discord_id = stardustvulpine::Utils::PadString(discord_id, 19);
                    fraction = stardustvulpine::Utils::PadString(fraction, 14);

                    table.append("| " + username + " | " + discord_id + " | " + fraction + " |\n");
                }
                table.append("```\n");

                const dpp::embed embed = dpp::embed()
                    .set_title(":bust_in_silhouette: Registered users")
                    .set_color(dpp::colors::sti_blue)
                    .set_description(table);

                event.reply(dpp::message().add_embed(embed));
            }
        )
        .AddSubCommand(
            "fractions",
            "List all fractions in database",
            {},
            [this](const dpp::slashcommand_t& event)
            {
                json allFractions;
                try {
                    allFractions = m_dbManager->GetAllFractions();
                } catch (SQLite::Exception &e) {
                    Log::Error("SQLite Error (" + std::to_string(e.getErrorCode()) + ") : " + e.what());
                }

                dpp::message replyMsg;

                for (const auto& fraction : allFractions)
                {
                    std::string fractionName = fraction.value("Name", "none");
                    std::string fractionDescription = fraction.value("Description", "none");
                    std::string fractionRoleID = fraction.value("DiscordRoleID", "none");
                    std::string fractionLevel = fraction.value("Level", "none");
                    std::string fractionCurrentExp = fraction.value("CurrentExp", "none");
                    std::string fractionExpToNextLevel = fraction.value("ExpToNextLevel", "none");

                    dpp::embed fractionEmbed = dpp::embed()
                        .set_color(dpp::colors::sti_blue)
                        .set_title(fractionName)
                        .set_description(fractionDescription)
                        .set_thumbnail(PLACEHOLDER_FRACTION_IMAGE)
                        .add_field("Level: " + fractionLevel, "(" + fractionCurrentExp + "/" + fractionExpToNextLevel + " exp)", true)
                        .add_field("Assigned Role: ", "<@&" + fractionRoleID + ">", true)
                        .set_footer(dpp::embed_footer("🟩🟩🟩🟩🟩🟩🟩🟩🟩🟩⬜⬜⬜⬜⬜⬜⬜⬜⬜⬜ 50%", "", ""));

                    replyMsg.add_embed(fractionEmbed);
                }

                if (Common::IsMessageEmpty(replyMsg)) {
                    event.reply(":x: No fractions created yet.");
                    return;
                }

                event.reply(replyMsg);
            }
        );

    //endregion
//endregion

//region TEST FIELD

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


//endregion
    }

    void Bot::OnMessageSentEventListen()
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
        Log::Trace("Updating bot commands...");
        OnCommandSentEvent();
        Log::Info("Commands Registered!");
        Log::Trace("Updating message listeners...");
        OnMessageSentEventListen();
        Log::Info("Listeners Updated!");
        Log::Info("Starting bot cluster...");
        mBotCluster.start(dpp::st_wait);
    }
}