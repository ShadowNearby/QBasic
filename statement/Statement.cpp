//
// Created by yanjs on 2022/11/9.
//

#include "Statement.h"
#include "Text.h"

void Statement::parse(QString &line)
{
    this->rowLine = line;
    auto c_line = line.toStdString();
    lexer.setBeg(c_line.c_str());
    for (auto token = lexer.next();
         !token.is_one_of(Token::Kind::End, Token::Kind::Unexpected);
         token = lexer.next()) {
        splitLine.push_back(QPair(token.lexeme().c_str(), token.kind()));
    }
}

Statement::Statement(QString &line)
{
    parse(line);
    type = setType(splitLine.at(1).first);
}

Statement::Statement(const Statement &statement)
{
    this->type = statement.type;
    this->rowLine = statement.rowLine;
    this->splitLine = statement.splitLine;
}

void Statement::parseLet()
{
    auto &varsTable = Text::variables;
}

void Statement::exec()
{
    switch (type) {
        case IF:
            executeIf();
            return;
        case REM:
            return;
        case INPUT:
            executeInput();
            return;
        case LET:
            executeLet();
            return;
        case PRINT:
            executePrint();
            return;
        case END:
            return;
        case GOTO:
            executeGoto();
            return;
        case WRONG:
            return;
    }
}

void Statement::executeInput()
{
    auto varName = splitLine.at(2).first;
    int value;
    std::cin >> value;
    Text::variables[varName] = value;
}

void Statement::executeIf()
{
    QVector<QPair<QString, Token::Kind>> leftExpr, rightExpr;
    QString strToLine;
    Token::Kind compareSign;
    bool isLeft = true;
    for (int it = 2; it < splitLine.size(); ++it) {
        auto cur = splitLine[it];
        if (cur.first == "THEN") {
            strToLine = splitLine[it + 1].first;
            break;
        }
        if (cur.second == Token::Kind::Equal
            || cur.second == Token::Kind::LessThan
            || cur.second == Token::Kind::GreaterThan) {
            compareSign = cur.second;
            isLeft = false;
            continue;
        }
        if (isLeft)
            leftExpr.push_back(cur);
        else
            rightExpr.push_back(cur);
    }
    int leftVal = calculateExp(leftExpr);
    int rightVal = calculateExp(rightExpr);
    int compareRes = calculateTwoNum(leftVal, rightVal, compareSign);
    if (compareRes) {
        int toLine = strToLine.toInt();
        Text::currentLineNum = toLine;
    } else {
        Text::currentLineNum = (Text::lines.find(Text::currentLineNum) + 1).key();
    }
}

void Statement::executePrint()
{
    QVector<QPair<QString, Token::Kind>> expr;
    for (int it = 2; it < splitLine.size(); ++it)
        expr.push_back(splitLine[it]);
    int value = calculateExp(expr);
    std::cout << value << std::endl;
}

void Statement::executeGoto()
{
    auto &strToLine = splitLine.at(2).first;
    int toLine = strToLine.toInt();
    Text::currentLineNum = toLine;
}

void Statement::executeLet()
{
    if (splitLine.at(2).second != Token::Kind::Identifier) {
        qDebug() << "Can not find var";
        return;
    }
    if (splitLine.at(3).second != Token::Kind::Equal) {
        qDebug() << "Can not find the equal sign";
        return;
    }
    auto varName = splitLine.at(2);
    QVector<QPair<QString, Token::Kind>> expr;
    for (int it = 4; it < splitLine.size(); ++it)
        expr.push_back(splitLine[it]);
    int value = calculateExp(expr);
    Text::variables[varName.first] = value;
}

StmtType Statement::setType(const QString &strType)
{
    if (strType == "GOTO")
        return GOTO;
    if (strType == "IF")
        return IF;
    if (strType == "LET")
        return LET;
    if (strType == "PRINT")
        return PRINT;
    if (strType == "END")
        return END;
    if (strType == "INPUT")
        return INPUT;
    if (strType == "REM")
        return REM;
    qDebug() << "Wrong key word";
    return WRONG;
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
    if (op == Token::Kind::LessThan)
        return a < b;
    if (op == Token::Kind::GreaterThan)
        return a > b;
    if (op == Token::Kind::Equal)
        return a == b;
    qDebug() << "operator error";
    return 0;
}

int calculateExp(QVector<QPair<QString, Token::Kind>> &expr)
{
    auto calSplitLine = expr;
    if (calSplitLine.first().second == Token::Kind::Minus)
        calSplitLine.push_front({"0", Token::Kind::Number});
    QVector<int> minusPos;
    for (auto it = 0; it < expr.size(); ++it) {
        if (expr[it].second == Token::Kind::LeftParen && expr[it + 1].second == Token::Kind::Minus) {
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
            || kind == Token::Kind::Slash) {
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
        if (kind == Token::Kind::Power) {
            if (BStack.empty()) {
                BStack.push_back(item);
                continue;
            }
            auto topOp = BStack.back();
            int currentPri = Priority[(int) kind];
            int topPri = Priority[(int) topOp.second];
            if (topPri > currentPri) {
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
