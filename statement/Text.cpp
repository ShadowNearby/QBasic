//
// Created by yanjs on 2022/11/12.
//

#include "Text.h"

QMap<int, Statement> Text::lines = QMap<int, Statement>();
QMap<QString, int> Text::variables = QMap<QString, int>();

void Text::input(std::string &line)
{
    auto inputText = QString(line.c_str());
    auto inputLines = inputText.split('\n');
    for (auto &inputLine: inputLines) {
        lexer.setBeg(inputLine.toStdString().c_str());
        auto firstToken = lexer.next();
        if (firstToken.is(Token::Kind::Number)) {
            lines[stoi(firstToken.lexeme())] = Statement(inputLine);
        } else {
            executeCommand(inputLine);
        }
    }
}

void Text::executeCommand(QString &command)
{
    if (command == "RUN")
        executeRun();
    else if (command == "LOAD")
        executeLoad();
    else if (command == "LIST")
        executeList();
    else if (command == "CLEAR")
        executeClear();
    else if (command == "HELP")
        executeHelp();
    else if (command == "QUIT")
        executeQuit();
    else {
        qDebug() << "ERROR!\tWRONG COMMAND!";
    }
}

void Text::executeLoad()
{
    qDebug() << "LOAD";
}

void Text::executeRun()
{
    qDebug() << "RUN";
}

void Text::executeList()
{
    qDebug() << "LIST";
}

void Text::executeClear()
{
    qDebug() << "CLEAR";
}

void Text::executeHelp()
{
    qDebug() << "HELP";
}

void Text::executeQuit()
{
    qDebug() << "QUIT";
}

Text::Text()
= default;

void test()
{
    std::string content = "100 REM Program to print the Fibonacci sequence.\n"
                          "110 LET max = 10000\n"
                          "120 LET n1 = 0\n"
                          "130 LET n2 = 1\n"
                          "140 IF n1 > max THEN 190\n"
                          "145 PRINT n1\n"
                          "150 LET n3 = n1 + n2\n"
                          "160 LET n1 = n2\n"
                          "170 LET n2 = n3\n"
                          "180 GOTO 140\n"
                          "190 END\n"
                          "QUIT\n"
                          "LI\n"
                          "LIST";
    Text text;
    text.input(content);
//    for (const auto &line: text.lines) {
//        qDebug() << line.splitLine;
//    }
}
//for (auto token = firstToken;
// !token.is_one_of(Token::Kind::End, Token::Kind::Unexpected);
//token = lexer.next()) {
//splitLine.push_back(QString(token.lexeme().c_str()));
//}