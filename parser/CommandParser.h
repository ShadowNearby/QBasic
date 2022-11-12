//
// Created by yanjs on 2022/11/9.
//

#pragma once

#include "../utility/QUtility.h"

enum Command
{
    RUN,
    LOAD,
    LIST,
    CLEAR,
    HELP,
    QUIT,
    WRONG
};

class CommandParser
{
public:
    Command parse(const QString &command);
};
