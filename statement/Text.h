//
// Created by yanjs on 2022/11/12.
//

#pragma once

#include "Statement.h"


class Text : public QThread
{
Q_OBJECT
public:

    Text(QObject *parent = nullptr);

    static bool waitForInput;
    static int currentLineNum;
    static QMap<int, Statement> lines;
    static QMap<QString, int> variables;

    void input(std::string &rowText);

    void input(QString &rowText);

    void input(const char *rowText);

    static void clear();

    void executeProgram();

    void run() override;

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

    void sendInputValue(int value);
};

void test();