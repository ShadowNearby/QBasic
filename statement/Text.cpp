//
// Created by yanjs on 2022/11/12.
//

#include "Text.h"

QMap<int, Statement> Text::lines = QMap<int, Statement>();
QMap<QString, int> Text::variables = QMap<QString, int>();
int Text::currentLineNum = 0;

void Text::input(std::string &line)
{
    auto inputText = QString(line.c_str());
    auto inputLines = inputText.split('\n');
    for (auto &inputLine: inputLines) {
        auto strInputLine = inputLine.toStdString();
        lexer.setBeg(strInputLine.c_str());
        auto firstToken = lexer.next();
        if (firstToken.is(Token::Kind::Number)) {
            int lineNum = stoi(firstToken.lexeme());
            lines[lineNum] = Statement(inputLine);
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

void Text::executeProgram()
{
    currentLineNum = lines.firstKey();
    for (;;) {
        auto currentLineIt = lines.find(currentLineNum);
        currentLineIt->exec();
        if (currentLineIt == lines.end() - 1)
            break;
        if (currentLineIt->type == GOTO || currentLineIt->type == IF)
            continue;
        currentLineNum = (currentLineIt + 1).key();
    }
}

Text::Text()
= default;

void test()
{
    QString filePath = "../basicSrc/case2";
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    Text text;
    auto allText = file.readAll().toStdString();
    text.input(allText);
    text.executeProgram();
    file.close();
}