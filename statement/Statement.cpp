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
    if (splitLine.size() < 2) {
        QString errorMsg = "error: syntax error in\n" + line;
        emit sendError(errorMsg);
        Text::error = true;
    }
    type = setType(splitLine.at(1).first);
}

Statement::Statement(QString &line)
{
    parse(line);
    if (type == WRONG)
        return;
    lineToTree();
}

Statement &Statement::operator=(const Statement &statement)
{
    this->type = statement.type;
    this->rowLine = statement.rowLine;
    this->splitLine = statement.splitLine;
    this->tree = statement.tree;
    return *this;
}

Statement::Statement(const Statement &statement)
{
    this->type = statement.type;
    this->rowLine = statement.rowLine;
    this->splitLine = statement.splitLine;
    this->tree = statement.tree;
}

bool Statement::exec()
{

    switch (type) {
        case IF:
            return executeIf();
        case REM:
            return false;
        case INPUT:
            active = true;
            executeInput();
            return false;
        case LET:
            executeLet();
            return false;
        case PRINT:
            executePrint();
            return false;
        case END:
            return false;
        case GOTO:
            executeGoto();
            return true;
        default:
            QString errorMsg = "error: statement type error in\n" + rowLine;
            emit sendError(errorMsg);
            Text::error = true;
            return false;
    }
}

void Statement::executeInput()
{
    Text::waitForInput = true;
    if (splitLine.size() != 3) {
        QString errorMsg = "error: expected syntax 'n INPUT variable' in\n" + rowLine;
        emit sendError(errorMsg);
        Text::error = true;
        return;
    }
    emit prepareInput();
}

bool Statement::executeIf()
{
    if (!checkIf())
        return false;
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
        QString errorMsg = "error: expected syntax 'n1 IF expression1 operator expression2 THEN n2' in\n" + rowLine;
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
            QString errorMsg = "error: expected syntax 'n1 IF expression1 operator expression2 THEN n2' in\n" + rowLine;
            emit sendError(errorMsg);
            Text::error = true;
            return true;
        }
        Text::currentLineNum = toLine;
        return true;
    }
    return false;
}

void Statement::executePrint()
{
    if (!checkPrint())
        return;
    QVector<QPair<QString, Token::Kind>> expr;
    for (int it = 2; it < splitLine.size(); ++it)
        expr.push_back(splitLine[it]);
    int value = calculateExp(expr);
    emit textPrint(value);
    QThread::usleep(1);
}

void Statement::executeGoto()
{
    if (!checkGoto())
        return;
    auto &strToLine = splitLine.at(2).first;
    int toLine = strToLine.toInt();
    Text::currentLineNum = toLine;
}

void Statement::executeLet()
{

    if (!checkLet())
        return;
    auto varName = splitLine.at(2);
    QVector<QPair<QString, Token::Kind>> expr;
    for (int it = 4; it < splitLine.size(); ++it)
        expr.push_back(splitLine[it]);
    int value = this->calculateExp(expr);
    Text::variables[varName.first] = value;
    test(expr);
}

void Statement::getInput(int value)
{
    if (!active)
        return;
    auto varName = splitLine.at(2).first;
    Text::variables[varName] = value;
    Text::waitForInput = false;
    active = false;
}

bool Statement::parseLet(QVector<QString> &exprTree)
{
    if (!checkLet())
        return false;
    exprTree.push_back(splitLine.first().first + " LET=");
    exprTree.push_back("    " + splitLine.at(2).first);
    QVector<QPair<QString, Token::Kind>> resTree;
    for (int i = 4; i < splitLine.size(); ++i)
        resTree.push_back(splitLine[i]);
    QVector<QString> tempTree;
    exprToTree(tempTree, resTree);
    for (const auto &item: tempTree)
        exprTree.push_back(item);
    return true;
}

bool Statement::parseGoto(QVector<QString> &exprTree)
{
    if (!checkGoto())
        return false;
    exprTree.push_back(splitLine.first().first + " GOTO");
    exprTree.push_back("    " + rowLine.split(" GOTO ").last());
    return true;
}

bool Statement::parseIf(QVector<QString> &exprTree)
{
    if (!checkIf())
        return false;
    exprTree.push_back(splitLine.first().first + " IF THEN");
    QVector<QPair<QString, Token::Kind>> leftExpr, rightExpr;
    QString strToLine;
    QString compareSign;
    bool isLeft = true;
    for (int it = 2; it < splitLine.size(); ++it) {
        auto cur = splitLine[it];
        if (cur.first == "THEN") {
            if (it + 2 != splitLine.size()) {
                QString errorMsg =
                        "error: expected syntax 'n1 IF expression1 operator expression2 THEN n2' in\n" + rowLine;
                emit sendError(errorMsg);
                Text::error = true;
                return false;
            }
            strToLine = splitLine[it + 1].first;
            break;
        }
        if (cur.second == Token::Kind::Equal
            || cur.second == Token::Kind::LessThan
            || cur.second == Token::Kind::GreaterThan) {
            compareSign = cur.first;
            isLeft = false;
            continue;
        }
        if (isLeft)
            leftExpr.push_back(cur);
        else
            rightExpr.push_back(cur);
    }
    QVector<QString> leftTree, rightTree;
    exprToTree(leftTree, leftExpr);
    exprToTree(rightTree, rightExpr);
    for (const auto &item: leftTree)
        exprTree.push_back(item);
    exprTree.push_back(compareSign);
    for (const auto &item: rightTree)
        exprTree.push_back(item);
    exprTree.push_back(splitLine.last().first);
    return true;
}

bool Statement::parsePrint(QVector<QString> &exprTree)
{
    if (!checkPrint())
        return false;
    exprTree.push_back(splitLine.first().first + " PRINT");
    QVector<QPair<QString, Token::Kind>> resTree;
    for (int i = 2; i < splitLine.size(); ++i)
        resTree.push_back(splitLine[i]);
    QVector<QString> tempTree;
    exprToTree(tempTree, resTree);
    for (const auto &item: tempTree)
        exprTree.push_back(item);
    return true;
}

bool Statement::parseRem(QVector<QString> &exprTree)
{
    if (!checkRem())
        return false;
    exprTree.push_back(splitLine.first().first + " REM");
    auto rem = rowLine.split(" REM ").last();
    exprTree.push_back("    " + rem);
    return true;
}

bool Statement::parseEnd(QVector<QString> &exprTree)
{
    if (!checkEnd())
        return false;
    exprTree.push_back(splitLine.first().first + " END");
    return true;
}

bool Statement::parseInput(QVector<QString> &exprTree)
{
    if (!checkInput())
        return false;
    exprTree.push_back(splitLine.first().first + " INPUT");
    exprTree.push_back("    " + rowLine.split(" INPUT ").last());
    return true;
}

void Statement::exprToTree(QVector<QString> &res, QVector<QPair<QString, Token::Kind>> &expr)
{
    QVector<QPair<QString, Token::Kind>> exprStack;
    expToPreStack(expr, exprStack);
    //    QVector<QPair<QString, Token::Kind>> resStack;
    //    QVector<QString> res;
    QVector<BTree *> treeStack, exprTreeStack;
    for (auto &item: exprStack)
        exprTreeStack.push_back(new BTree(item));
    BTree *cur;
    for (auto &node: exprTreeStack) {
        auto &kind = node->value.second;
        //        auto &content = item.first;
        if (kind == Token::Kind::Identifier || kind == Token::Kind::Number) {
            treeStack.push_back(node);
        } else if (isOperator(kind)) {
            if (treeStack.empty()) {
                QString errorMsg = "error: expression error in\n" + rowLine;
                Text::error = true;
                emit sendError(errorMsg);
                return;
            }
            auto var1 = treeStack.back();
            treeStack.pop_back();
            if (treeStack.empty()) {
                QString errorMsg = "error: expression error in\n" + rowLine;
                Text::error = true;
                emit sendError(errorMsg);
                return;
            }
            auto var2 = treeStack.back();
            treeStack.pop_back();
            cur = node;
            cur->left = var2;
            cur->right = var1;
            treeStack.push_back(cur);
        }

    }
    //    res.push_front(resStack.first().first);
    QQueue<BTree *> queue;
    queue.enqueue(treeStack.first());
    QMap<int, QVector<QString>> viewRes;
    QString posOffset = "    ";
    viewTree(treeStack.first(), viewRes, 0);
    for (const auto &row: viewRes) {
        for (const auto &item: row) {
            res.push_back(posOffset + item);
        }
        posOffset += ' ';
    }
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

void Statement::expToPreStack(QVector<QPair<QString, Token::Kind>> &expr, QVector<QPair<QString, Token::Kind>> &res)
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
    QVector<QPair<QString, Token::Kind>> BStack;
    for (const auto &item: calSplitLine) {
        auto &kind = item.second;
        if (kind == Token::Kind::Number || kind == Token::Kind::Identifier) {
            res.push_back(item);
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
                res.push_back(topOp);
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
                    res.push_back(topOp);
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
                    res.push_back(topOp);
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
        res.push_back(topOp);
        BStack.pop_back();
    }
}

int Statement::calculateExp(QVector<QPair<QString, Token::Kind>> &expr)
{
    QVector<QPair<QString, Token::Kind>> AStack;
    expToPreStack(expr, AStack);
    auto &variables = Text::variables;
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
    Text::error = true;
    return WRONG;
}

void Statement::lineToTree()
{
    QVector<QString> exprTree;
    switch (type) {
        case IF:
            if (!parseIf(exprTree))
                return;
            break;
        case REM:
            if (!parseRem(exprTree))
                return;
            break;
        case INPUT:
            if (!parseInput(exprTree))
                return;
            break;
        case LET:
            if (!parseLet(exprTree))
                return;
            break;
        case PRINT:
            if (!parsePrint(exprTree))
                return;
            break;
        case END:
            if (!parseEnd(exprTree))
                return;
            break;
        case GOTO:
            if (!parseGoto(exprTree))
                return;
            break;
        default:
            return;
    }
    for (int i = 0; i < exprTree.size() - 1; ++i) {
        this->tree += exprTree[i] + "\n";
    }
    this->tree += exprTree.last();
}

bool Statement::checkGoto()
{
    if (!rowLine.contains("GOTO")
        || splitLine.size() != 3
        || splitLine.first().second != Token::Kind::Number
        || splitLine.last().second != Token::Kind::Number) {
        QString errorMsg = "error: expected syntax 'n1 GOTO n2' in\n" + rowLine;
        emit sendError(errorMsg);
        Text::error = true;
        return false;
    }
    return true;
}

bool Statement::checkLet()
{
    if (!rowLine.contains("LET") || !rowLine.contains("=")
        || splitLine.first().second != Token::Kind::Number
        || splitLine.size() <= 4
        || splitLine.at(3).second != Token::Kind::Equal) {
        QString errorMsg = "error: expected syntax 'n1 GOTO n2' in\n" + rowLine;
        emit sendError(errorMsg);
        Text::error = true;
        return false;
    }
    return true;
}

bool Statement::checkIf()
{
    if (!rowLine.contains("IF") || !rowLine.contains("THEN")
        || splitLine.size() <= 5
        || !(rowLine.contains(">") || rowLine.contains("=") || rowLine.contains("<"))
        || splitLine.first().second != Token::Kind::Number
        || splitLine.last().second != Token::Kind::Number) {
        QString errorMsg = "error: expected syntax 'n1 IF expression1 operator expression2 THEN n2' in\n" + rowLine;
        emit sendError(errorMsg);
        Text::error = true;
        return false;
    }
    return true;
}

bool Statement::checkPrint()
{
    if (!rowLine.contains("PRINT") || splitLine.size() <= 2
        || splitLine.first().second != Token::Kind::Number) {
        QString errorMsg = "error: expected syntax 'n PRINT expression' in\n" + rowLine;
        emit sendError(errorMsg);
        Text::error = true;
        return false;
    }
    return true;
}

bool Statement::checkRem()
{
    if (!rowLine.contains("REM")
        || splitLine.size() <= 2
        || splitLine.first().second != Token::Kind::Number) {
        QString errorMsg = "error: expected syntax 'n REM ...' in\n" + rowLine;
        emit sendError(errorMsg);
        Text::error = true;
        return false;
    }
    return true;
}

bool Statement::checkEnd()
{
    if (!rowLine.contains("END")
        || splitLine.size() < 2
        || splitLine.first().second != Token::Kind::Number) {
        QString errorMsg = "error: expected syntax 'n END' in\n" + rowLine;
        emit sendError(errorMsg);
        Text::error = true;
        return false;
    }
    return true;
}

bool Statement::checkInput()
{
    if (!rowLine.contains("INPUT")
        || splitLine.size() != 3
        || splitLine.first().second != Token::Kind::Number) {
        QString errorMsg = "error: expected syntax 'n INPUT variable' in\n" + rowLine;
        emit sendError(errorMsg);
        Text::error = true;
        return false;
    }
    return true;
}

void Statement::test(QVector<QPair<QString, Token::Kind>> &expr)
{

//    while (!queue.empty()) {
//        BTree *node;
//        node = queue.dequeue();
//        std::cout << node->value.first.toStdString();
//        if (!queue.empty() || node->left || node->right)
//            std::cout << " ";
//        if (node->left)
//            queue.enqueue(node->left);
//        if (node->right)
//            queue.enqueue(node->right);
//    }
    //    QString posOffset = "    ";
    //    for (int i = 0; i < res.size(); ++i) {
    //        if (i % 2 != 0)
    //            posOffset += ' ';
    //        res[i] = posOffset + res[i];
    //    }
}

void Statement::viewTree(BTree *root, QMap<int, QVector<QString>> &res, int deep)
{
    if (!root)
        return;
    if (res.find(deep) == res.end())
        res[deep] = QVector<QString>(1, root->value.first);
    else {
        res[deep].push_back(root->value.first);
    }
    if (root->left) viewTree(root->left, res, deep + 1);
    if (root->right) viewTree(root->right, res, deep + 1);
}

bool isOperator(Token::Kind kind)
{
    return kind == Token::Kind::Plus
           || kind == Token::Kind::Minus
           || kind == Token::Kind::Asterisk
           || kind == Token::Kind::Slash
           || kind == Token::Kind::Power;
}
