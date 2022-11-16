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

    ~Text() override;

    static bool waitForInput;
    static int currentLineNum;
    static QMap<int, Statement> lines;
    static QMap<QString, int> variables;
    static bool error;

    void input(std::string &rowText);

    void input(QString &rowText);

    void input(const char *rowText);

    void clear();

    void executeProgram();

    void run() override;


signals:

    void sendError(QString &error);
};

void test();