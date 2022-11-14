//
// Created by yanjs on 2022/11/9.
//

#include "Statement.h"
#include "Text.h"

Lexer Statement::lexer;
QMap<int, int> Statement::Priority = QMap<int, int>(
        {{(int) Token::Kind::LeftParen,  0},
         {(int) Token::Kind::Plus,       1},
         {(int) Token::Kind::Minus,      1},
         {(int) Token::Kind::Asterisk,   2},
         {(int) Token::Kind::Slash,      2},
         {(int) Token::Kind::Power,      2},
         {(int) Token::Kind::RightParen, 3}});

//QString type_str;
//try {
//type_str = splitLine.at(1).first;
//} catch (std::exception &e) {
//qDebug() << e.what();
//}
//if (type_str == "LET")
//type = LET;
//else if (type_str == "END")
//type = END;
//else if (type_str == "INPUT")
//type = INPUT;
//else if (type_str == "IF")
//type = IF;
//else if (type_str == "GOTO")
//type = GOTO;
//else if (type_str == "PRINT")
//type = PRINT;
//else if (type_str == "REM")
//type = REM;
//else {
//qDebug() << "error";
//type = WRONG;
//}
void Statement::parse(QString &line)
{
    this->rowLine = line;
    type = WRONG;
    auto c_line = line.toStdString();
    lexer.setBeg(c_line.c_str());
    for (auto token = lexer.next();
         !token.is_one_of(Token::Kind::End, Token::Kind::Unexpected);
         token = lexer.next()) {
        splitLine.push_back(QPair(token.lexeme().c_str(), token.kind()));
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
    parse(line);
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

int calculateTwoNum(int a, int b, const Token::Kind &op)
{
    if (op == Token::Kind::Plus)
        return a + b;
    if (op == Token::Kind::Minus)
        return a - b;
    if (op == Token::Kind::Asterisk)
        return a * b;
    if (op == Token::Kind::Slash)
        return a / b;
    if (op == Token::Kind::Power)
        return (int) std::pow(a, b);
    qDebug() << "operator error";
    return 0;
}

// QVector<QPair<QString, Token::Kind>> &expr
int Statement::calculateExp()
{
    auto calSplitLine = splitLine;
    if (calSplitLine.first().second == Token::Kind::Minus)
        calSplitLine.push_front({"0", Token::Kind::Number});
    QVector<int> minusPos;
    for (auto it = 0; it < splitLine.size(); ++it) {
        if (splitLine[it].second == Token::Kind::LeftParen && splitLine[it + 1].second == Token::Kind::Minus) {
            minusPos.push_back(it + 1);
        }
    }
    for (int i = 0; i < minusPos.size(); ++i) {
        calSplitLine.insert(i + minusPos[i], {"0", Token::Kind::Number});
    }
    QVector<QPair<QString, Token::Kind>> AStack;
    QVector<QPair<QString, Token::Kind>> BStack;
    auto &variables = Text::variables;
    for (const auto &item: calSplitLine) {
        auto &kind = item.second;
        auto &content = item.first;
        if (kind == Token::Kind::Number || kind == Token::Kind::Identifier) {
            AStack.push_back(item);
            continue;
        }
        if (kind == Token::Kind::LeftParen) {
            BStack.push_back(item);
            continue;
        }
        if (kind == Token::Kind::RightParen) {
            while (true) {
                auto topOp = BStack.back();
                BStack.pop_back();
                if (topOp.second == Token::Kind::LeftParen)
                    break;
                AStack.push_back(topOp);
            }
            continue;
        }
        if (kind == Token::Kind::Plus
            || kind == Token::Kind::Minus
            || kind == Token::Kind::Asterisk
            || kind == Token::Kind::Slash
            || kind == Token::Kind::Power) {
            if (BStack.empty()) {
                BStack.push_back(item);
                continue;
            }
            auto topOp = BStack.back();
            int currentPri = Priority[(int) kind];
            int topPri = Priority[(int) topOp.second];
            if (topPri >= currentPri) {
                while (true) {
                    BStack.pop_back();
                    AStack.push_back(topOp);
                    if (BStack.empty())
                        break;
                    topOp = BStack.back();
                    topPri = Priority[(int) topOp.second];
                    if (topPri < currentPri)
                        break;
                }
            }
            BStack.push_back(item);
            continue;
        }
    }
    while (!BStack.empty()) {
        auto topOp = BStack.back();
        AStack.push_back(topOp);
        BStack.pop_back();
    }
    QVector<int> answerStack;
    for (const auto &item: AStack) {
        auto &kind = item.second;
        auto &content = item.first;
        if (kind == Token::Kind::Number)
            answerStack.push_back(content.toInt());
        else if (kind == Token::Kind::Identifier) {
            if (variables.find(content) == variables.end()) {
                qDebug() << "error! undefined var " + content;
                break;
            }
            answerStack.push_back(variables[content]);
        } else if (kind == Token::Kind::Plus
                   || kind == Token::Kind::Minus
                   || kind == Token::Kind::Asterisk
                   || kind == Token::Kind::Slash
                   || kind == Token::Kind::Power) {
            auto var1 = answerStack.back();
            try {
                answerStack.pop_back();
            } catch (std::exception &e) {
                qDebug() << e.what();
                break;
            }
            auto var2 = answerStack.back();
            try {
                answerStack.pop_back();
            } catch (std::exception &e) {
                qDebug() << e.what();
                break;
            }
            answerStack.push_back(calculateTwoNum(var2, var1, kind));
        }
    }
    return answerStack.first();
}
