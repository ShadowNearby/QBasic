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

    [[nodiscard]] bool executeInput();

    [[nodiscard]] bool executeLet();

    bool executeIf();

    bool executePrint();

    [[nodiscard]] bool executeGoto();

    void parseLet(QVector<QString> &);

    void parseGoto(QVector<QString> &);

    void parseIf(QVector<QString> &);

    void parsePrint(QVector<QString> &);

    int calculateExp(QVector<QPair<QString, Token::Kind>> &expr);

    StmtType setType(const QString &strType);

    int calculateTwoNum(int a, int b, const Token::Kind &op);

    void exprToTree(QString &res, QVector<QPair<QString, Token::Kind>> &expr);

signals:

    void textPrint(int value);

    void prepareInput();

    void sendError(QString error);

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


