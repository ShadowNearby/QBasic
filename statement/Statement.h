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


    explicit Statement(QString &line);


    Statement(const Statement &statement);

    QString rowLine;
    QVector<QPair<QString, Token::Kind>> splitLine;
    StmtType type;

    void parse(QString &line);

    StmtType setType(const QString &strType);

    void exec();

    void executeInput();

    void executeLet();

    void executeIf();

    void executePrint();

    void executeGoto();

    void parseLet();
};

static Lexer lexer;
static const QMap<int, int> Priority = QMap<int, int>(
        {{(int) Token::Kind::LeftParen,  0},
         {(int) Token::Kind::Plus,       1},
         {(int) Token::Kind::Minus,      1},
         {(int) Token::Kind::Asterisk,   2},
         {(int) Token::Kind::Slash,      2},
         {(int) Token::Kind::Power,      3},
         {(int) Token::Kind::RightParen, 4}});

int calculateExp(QVector<QPair<QString, Token::Kind>> &expr);

int calculateTwoNum(int a, int b, const Token::Kind &op);