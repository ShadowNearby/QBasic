//
// Created by yanjs on 2022/11/12.
//

#pragma once

#include "Statement.h"


class Text : public QObject
{
    Lexer lexer;
public:
    Text();

    static QMap<int, Statement> lines;
    static QMap<QString, int> variables;

    void input(std::string &line);

    void executeCommand(QString &command);

signals:

    void executeRun();

    void executeLoad();

    void executeList();

    void executeClear();

    void executeHelp();

    void executeQuit();


};

void test();