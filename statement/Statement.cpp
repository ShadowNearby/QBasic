//
// Created by yanjs on 2022/11/9.
//

#include "Statement.h"
#include "Text.h"

Lexer Statement::lexer = Lexer();
QMap<QString, int> Statement::priority = QMap<QString, int>
        ({
                 {")", 0},
                 {"+", 1},
                 {"-", 1},
                 {"*", 2},
                 {"/", 2},
                 {"**", 2},
                 {"(", 3}});

void Statement::parse()
{
    QString type_str;
    try {
        type_str = splitLine.at(1);
    } catch (std::exception &e) {
        qDebug() << e.what();
    }
    if (type_str == "LET")
        type = LET;
    else if (type_str == "END")
        type = END;
    else if (type_str == "INPUT")
        type = INPUT;
    else if (type_str == "IF")
        type = IF;
    else if (type_str == "GOTO")
        type = GOTO;
    else if (type_str == "PRINT")
        type = PRINT;
    else if (type_str == "REM")
        type = REM;
    else {
        qDebug() << "error";
        type = WRONG;
    }
}

void Statement::executeInput()
{

}

void Statement::executeLet()
{

}

Statement::Statement(QString &line)
{
    this->rowLine = line;
    type = WRONG;
    lexer.setBeg(line.toStdString().c_str());
    for (auto token = lexer.next();
         !token.is_one_of(Token::Kind::End, Token::Kind::Unexpected);
         token = lexer.next()) {
        splitLine.push_back(token.lexeme().c_str());
    }
}

Statement::Statement(const Statement &statement)
{
    this->type = statement.type;
    this->rowLine = statement.rowLine;
    this->splitLine = statement.splitLine;
}

void Statement::parserLet()
{
    auto &varsTable = Text::variables;

}

int calculateTwoNum(int a, int b, QString &op)
{
    if (op == "+")
        return a + b;
    if (op == "-")
        return a - b;
    if (op == "*")
        return a * b;
    if (op == "/")
        return a / b;
    if (op == "**")
        return (int) std::pow(a, b);
}

int Statement::calculateExp(QVector<QString> &exp)
{
    QVector<QString> op;
    QVector<int> vals;
    return 0;
}








