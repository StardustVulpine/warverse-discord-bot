//
// Created by stardustvulpine on 3/26/26.
//

#include "Bot.hpp"
#include <Common.hpp>

int main ()
{
    stardustvulpine::Utils::Console::Log::ToFile(wdb::Common::GetLogsDir());
    wdb::discord::Bot bot;
    bot.Start();

    return 0;
}
