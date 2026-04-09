//
// Created by stardustvulpine on 3/26/26.
//

#include "Bot.hpp"

int main ()
{
    warverse::Bot bot;
    bot.UpdateCommands();
    bot.RegisterCommands();
    bot.Start();

    return 0;
}