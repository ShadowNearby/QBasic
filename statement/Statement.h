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


class Statement : public QObject
{
Q_OBJECT
public:
    Statement() : type(WRONG)
    {};


    Statement(QString &line);


    Statement(const Statement &statement);

    Statement &operator=(const Statement &statement);

    QString rowLine;
    QVector<QPair<QString, Token::Kind>> splitLine;
    StmtType type;

    void parse(QString &line);

    bool exec();

    [[nodiscard]] bool executeInput() const;

    [[nodiscard]] bool executeLet() const;

    bool executeIf();

    bool executePrint();

    [[nodiscard]] bool executeGoto() const;

    void parseLet();

signals:

    void textPrint(int value);

public slots:

    void getInput(int value);

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

StmtType setType(const QString &strType);

int calculateTwoNum(int a, int b, const Token::Kind &op);