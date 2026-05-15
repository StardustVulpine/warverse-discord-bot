//
// Created by stardustvulpine on 3/26/26.
//

#include "Bot.hpp"
#include <Common.hpp>

int main ()
{
    stardustvulpine::Utils::Console::Log::ToFile(wdb::Common::GetLogsDir());

    wdb::Bot bot;
    bot.Run();

    return 0;
}
