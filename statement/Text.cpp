//
// Created by yanjs on 2022/11/12.
//

#include "Text.h"

QMap<int, Statement> Text::lines = QMap<int, Statement>();
QMap<QString, int> Text::variables = QMap<QString, int>();
auto Text::lexer = Lexer();

void Text::input(std::string &line)
{
    auto inputText = QString(line.c_str());
    auto inputLines = inputText.split('\n');
    for (auto &inputLine: inputLines) {
        auto strInputLine = inputLine.toStdString();
        lexer.setBeg(strInputLine.c_str());
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

