//
// Created by yanjs on 2022/11/12.
//

#include "Text.h"

QMap<int, Statement> Text::lines = QMap<int, Statement>();
QMap<QString, int> Text::variables = QMap<QString, int>();
int Text::currentLineNum = 0;
bool Text::waitForInput = false;
bool Text::error = false;

void Text::input(std::string &rowText)
{
    auto inputText = QString(rowText.c_str());
    input(inputText);
}

void Text::input(const char *rowText)
{
    QString inputText(rowText);
    input(inputText);
}

void Text::input(QString &rowText)
{
    auto inputLines = rowText.split('\n');
    for (auto &inputLine: inputLines) {
        auto strInputLine = inputLine.toStdString();
        lexer.setBeg(strInputLine.c_str());
        auto firstToken = lexer.next();
        if (firstToken.is(Token::Kind::Number)) {
            int lineNum = stoi(firstToken.lexeme());
            Statement stmt(inputLine);
            lines[lineNum] = stmt;
        }
    }
}

void Text::executeProgram()
{
    currentLineNum = lines.firstKey();
    while (true) {
        QThread::usleep(1);
        if (waitForInput)
            continue;
        auto currentLineIt = lines.find(currentLineNum);
        auto nextLine = !currentLineIt->exec();
        if ((currentLineIt == lines.end() - 1 && nextLine) || error)
            break;
        if (nextLine)
            currentLineNum = (currentLineIt + 1).key();
    }
}

void Text::clear()
{
    currentLineNum = 0;
    lines.clear();
    variables.clear();
}

Text::Text(QObject *parent)
{
}

void Text::run()
{
    executeProgram();
}

