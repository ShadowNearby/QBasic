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
    bool active = false;


    Statement() : type(WRONG)
    {};


    Statement(QString &line);


    Statement(const Statement &statement);

    Statement &operator=(const Statement &statement);

    QString tree;
    QString rowLine;
    QVector<QPair<QString, Token::Kind>> splitLine;
    StmtType type;


    void parse(QString &line);

    bool exec();

    void executeInput();

    void executeLet();

    bool executeIf();

    void executePrint();

    void executeGoto();

    void lineToTree();

    bool parseLet(QVector<QString> &);

    bool parseGoto(QVector<QString> &);

    bool parseIf(QVector<QString> &);

    bool parsePrint(QVector<QString> &);

    bool parseRem(QVector<QString> &);

    bool parseEnd(QVector<QString> &);

    bool parseInput(QVector<QString> &);

    bool checkLet();

    bool checkGoto();

    bool checkIf();

    bool checkPrint();

    bool checkRem();

    bool checkEnd();

    bool checkInput();


    int calculateExp(QVector<QPair<QString, Token::Kind>> &expr);

    StmtType setType(const QString &strType);

    int calculateTwoNum(int a, int b, const Token::Kind &op);

    void exprToTree(QVector<QString> &res, QVector<QPair<QString, Token::Kind>> &expr);

    void expToPreStack(QVector<QPair<QString, Token::Kind>> &expr, QVector<QPair<QString, Token::Kind>> &res);

signals:

    void textPrint(int value);

    void prepareInput();

    void sendError(QString error);

public slots:

    void getInput(int value);

};

bool isOperator(Token::Kind kind);

static Lexer lexer;
static const QMap<int, int> Priority = QMap<int, int>(
        {{(int) Token::Kind::LeftParen,  0},
         {(int) Token::Kind::Plus,       1},
         {(int) Token::Kind::Minus,      1},
         {(int) Token::Kind::Asterisk,   2},
         {(int) Token::Kind::Slash,      2},
         {(int) Token::Kind::Power,      3},
         {(int) Token::Kind::RightParen, 4}});


