//
// Created by yanjs on 2022/11/12.
//

#include "Text.h"

QMap<int, Statement> Text::lines = QMap<int, Statement>();
QMap<QString, int> Text::variables = QMap<QString, int>();
int Text::currentLineNum = 0;

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
        } else {
            executeCommand(inputLine);
        }
    }
}


void Text::executeCommand(QString &command)
{
    auto stdCommand = command.toStdString();
    lexer.setBeg(stdCommand.c_str());
    QVector<QPair<QString, Token::Kind>> splitCommand;
    for (auto token = lexer.next();
         !token.is_one_of(Token::Kind::End, Token::Kind::Unexpected);
         token = lexer.next()) {
        splitCommand.push_back(QPair(token.lexeme().c_str(), token.kind()));
    }
    if (splitCommand.empty())
        return;
    if (splitCommand.first().second == Token::Kind::Number) {
        if (splitCommand.size() == 1) {
            int removeLineNum = command.toInt();
            lines.remove(removeLineNum);
            emit resetCodeText();
            return;
        }
        int insertLineNum = splitCommand.first().first.toInt();
        lines[insertLineNum] = Statement(command);
        emit resetCodeText();
        return;
    }
    if (command == "RUN") { emit sendRun(); }
    else if (command == "LOAD") { emit sendLoad(); }
    else if (command == "LIST") { emit sendList(); }
    else if (command == "CLEAR") { emit sendClear(); }
    else if (command == "HELP") { emit sendHelp(); }
    else if (command == "QUIT") { emit sendQuit(); }
    else {
        qDebug() << "ERROR!\tWRONG COMMAND!";
    }
}


void Text::executeProgram()
{
    currentLineNum = lines.firstKey();
    for (;;) {
        auto currentLineIt = lines.find(currentLineNum);
        auto nextLine = !currentLineIt->exec();
        if (currentLineIt == lines.end() - 1 && nextLine)
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