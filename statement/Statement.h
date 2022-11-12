//
// Created by yanjs on 2022/11/9.
//

#pragma once

#include "../utility/QUtility.h"
#include "../parser/Lexer.h"

enum StmtType
{
    REM,
    INPUT,
    LET,
    PRINT,
    END,
    IF,
    GOTO,
    WRONG
};


class Statement
{
public:
    Statement() : type(WRONG)
    {};
    static Lexer lexer;

    explicit Statement(QString &line);

    static QMap<QString, int> priority;

    Statement(const Statement &statement);

    QString rowLine;
    QVector<QString> splitLine;
    StmtType type;

    void parse();

    int calculateExp(QVector<QString> &);

    void executeInput();

    void executeLet();

    void parserLet();
};

