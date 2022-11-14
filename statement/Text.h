//
// Created by yanjs on 2022/11/12.
//

#pragma once

#include "Statement.h"


class Text : public QObject
{
Q_OBJECT
public:

    Text();

    static int currentLineNum;
    static QMap<int, Statement> lines;
    static QMap<QString, int> variables;

    void input(std::string &rowText);

    void input(QString &rowText);

    void input(const char *rowText);

    static void clear();

    void executeProgram();

public slots:

    void executeCommand(QString &command);

signals:

    void sendRun();

    void sendLoad();

    void sendList();

    void sendClear();

    void sendHelp();

    void sendQuit();

    void resetCodeText();

};

void test();