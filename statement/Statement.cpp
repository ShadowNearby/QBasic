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


bool Statement::exec()
{
    switch (type) {
        case IF:
            return executeIf();
        case REM:
            return false;
        case INPUT:
            return executeInput();
        case LET:
            return executeLet();
        case PRINT:
            return executePrint();
        case END:
            return false;
        case GOTO:
            return executeGoto();
        default:
            return false;
    }
}

bool Statement::executeInput()
{
    Text::waitForInput = true;
    if (splitLine.size() != 3) {
        QString errorMsg = "error: expected syntax 'n INPUT variable' in\n" + rowLine;
        emit sendError(errorMsg);
        Text::error = true;
        return false;
    }
    emit prepareInput();
    return false;
}

bool Statement::executeIf()
{
    if (splitLine.size() <= 6) {
        QString errorMsg =
                "error: expected syntax 'n1 IF expression1 operator expression2 THEN n2' in\n" + rowLine;
        emit sendError(errorMsg);
        Text::error = true;
        return true;
    }
    QVector<QPair<QString, Token::Kind>> leftExpr, rightExpr;
    QString strToLine;
    Token::Kind compareSign;
    bool isLeft = true;
    for (int it = 2; it < splitLine.size(); ++it) {
        auto cur = splitLine[it];
        if (cur.first == "THEN") {
            if (it + 2 != splitLine.size()) {
                QString errorMsg =
                        "error: expected syntax 'n1 IF expression1 operator expression2 THEN n2' in\n" + rowLine;
                emit sendError(errorMsg);
                Text::error = true;
                return true;
            }
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
    if (leftExpr.empty() || rightExpr.empty()) {
        QString errorMsg =
                "error: expected syntax 'n1 IF expression1 operator expression2 THEN n2' in\n" + rowLine;
        emit sendError(errorMsg);
        Text::error = true;
        return true;
    }
    int leftVal = calculateExp(leftExpr);
    int rightVal = calculateExp(rightExpr);
    int compareRes = calculateTwoNum(leftVal, rightVal, compareSign);
    if (compareRes) {
        int toLine = strToLine.toInt();
        if (Text::lines.find(toLine) == Text::lines.end()) {
            QString errorMsg =
                    "error: expected syntax 'n1 IF expression1 operator expression2 THEN n2' in\n" + rowLine;
            emit sendError(errorMsg);
            Text::error = true;
            return true;
        }
        Text::currentLineNum = toLine;
        return true;
    }
    return false;
}

bool Statement::executePrint()
{
    if (splitLine.size() <= 2) {
        QString errorMsg = "error: expected syntax 'n PRINT expression' in\n" + rowLine;
        emit sendError(errorMsg);
        Text::error = true;
        return true;
    }
    QVector<QPair<QString, Token::Kind>> expr;
    for (int it = 2; it < splitLine.size(); ++it)
        expr.push_back(splitLine[it]);
    int value = calculateExp(expr);
    emit textPrint(value);
    QThread::usleep(1);
    return false;
}

bool Statement::executeGoto()
{
    if (splitLine.size() != 3) {
        QString errorMsg = "error: expected syntax 'n1 GOTO n2' in\n" + rowLine;
        emit sendError(errorMsg);
        Text::error = true;
        return true;
    }
    auto &strToLine = splitLine.at(2).first;
    int toLine = strToLine.toInt();
    Text::currentLineNum = toLine;
    return true;
}

bool Statement::executeLet()
{

    if (splitLine.size() <= 4) {
        QString errorMsg = "error: expected syntax 'n LET variable = expression' in\n" + rowLine;
        emit sendError(errorMsg);
        Text::error = true;
        return false;
    }
    if (splitLine.at(3).second != Token::Kind::Equal) {
        QString errorMsg = "error: expected '=' token in\n" + rowLine;
        emit sendError(errorMsg);
        Text::error = true;
        return false;
    }
    if (splitLine.at(2).second != Token::Kind::Identifier) {
        QString errorMsg = "error: expected primary-expression before '=' token in \n" + rowLine;
        emit sendError(errorMsg);
        Text::error = true;
        return false;
    }
    auto varName = splitLine.at(2);
    QVector<QPair<QString, Token::Kind>> expr;
    for (int it = 4; it < splitLine.size(); ++it)
        expr.push_back(splitLine[it]);
    int value = this->calculateExp(expr);
    Text::variables[varName.first] = value;
    return false;
}


void Statement::getInput(int value)
{
    auto varName = splitLine.at(2).first;
    Text::variables[varName] = value;
    Text::waitForInput = false;
}

Statement &Statement::operator=(const Statement &statement)
{
    this->type = statement.type;
    this->rowLine = statement.rowLine;
    this->splitLine = statement.splitLine;
    return *this;
}

void Statement::parseLet(QVector<QString> &exprTree)
{
    QString res = "LET\n";
}

void Statement::parseGoto(QVector<QString> &exprTree)
{
    exprTree.push_back("GOTO\n");
    exprTree.push_back(splitLine.at(2).first);
}

void Statement::parseIf(QVector<QString> &exprTree)
{
    QString res = "IF\n";
}

void Statement::parsePrint(QVector<QString> &exprTree)
{
    QString res = "PRINT\n";
}

void Statement::exprToTree(QString &res, QVector<QPair<QString, Token::Kind>> &expr)
{

}

int Statement::calculateTwoNum(int a, int b, const Token::Kind &op)
{
    switch (op) {
        case Token::Kind::Plus:
            return a + b;
        case Token::Kind::Minus:
            return a - b;
        case Token::Kind::Asterisk:
            return a * b;
        case Token::Kind::Slash:
            return a / b;
        case Token::Kind::Power:
            return (int) std::pow(a, b);
        case Token::Kind::LessThan:
            return a < b;
        case Token::Kind::GreaterThan:
            return a > b;
        case Token::Kind::Equal:
            return a == b;
        default:
            QString errorMsg = "error: unknown operator in\n" + rowLine;
            emit Statement::sendError(errorMsg);
            Text::error = true;
            return -1;
    }


}

int Statement::calculateExp(QVector<QPair<QString, Token::Kind>> &expr)
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
                QString errorMsg = "error: '" + content + "'" + " was not declared in this scope\n" + rowLine;
                Text::error = true;
                emit sendError(errorMsg);
                return -1;
            }
            answerStack.push_back(variables[content]);
        } else if (kind == Token::Kind::Plus
                   || kind == Token::Kind::Minus
                   || kind == Token::Kind::Asterisk
                   || kind == Token::Kind::Slash
                   || kind == Token::Kind::Power) {
            if (answerStack.empty()) {
                QString errorMsg = "error: expression error in\n" + rowLine;
                Text::error = true;
                emit sendError(errorMsg);
                return -1;
            }
            auto var1 = answerStack.back();
            answerStack.pop_back();
            if (answerStack.empty()) {
                QString errorMsg = "error: expression error in\n" + rowLine;
                Text::error = true;
                emit sendError(errorMsg);
                return -1;
            }
            auto var2 = answerStack.back();
            answerStack.pop_back();
            int res = calculateTwoNum(var2, var1, kind);
            if (Text::error)
                return -1;
            answerStack.push_back(res);
        }
    }
    return answerStack.first();
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
    QString errorMsg = "error: statement type error in\n" + rowLine;
    emit sendError(errorMsg);
    Text::error = true;
    return WRONG;
}
