//
// Created by stardustvulpine on 3/26/26.
//

#include "Bot.hpp"
#include <Common.hpp>

int main ()
{
    stardustvulpine::Utils::Console::Log::ToFile(wdb::Common::GetLogsDir());

    wdb::Bot bot;

    Log::Debug(std::string(bot.GetDBManager()->get()->GetAllUsers()));
    Log::Debug(std::string(bot.GetDBManager()->get()->GetAllFractions()));

    bot.Run();

    return 0;
}
