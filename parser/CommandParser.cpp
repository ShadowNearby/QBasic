//
// Created by yanjs on 2022/11/9.
//

#include "CommandParser.h"

Command CommandParser::parse(const QString &command)
{
    if (command == QString("RUN"))
        return RUN;
    else if (command == QString("LOAD"))
        return LOAD;
    else if (command == QString("LIST"))
        return LIST;
    else if (command == QString("CLEAR"))
        return CLEAR;
    else if (command == QString("HELP"))
        return HELP;
    else if (command == QString("QUIT"))
        return QUIT;
    else
        return WRONG;

}
