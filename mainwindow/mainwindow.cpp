#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    text = new Text(this);
    connect(ui->cmdLineEdit, &QLineEdit::editingFinished, this, &MainWindow::cmdLineEdit_editingFinished);
    connect(ui->btnClearCode, &QPushButton::clicked, this, &MainWindow::btnClearCode_clicked);
    connect(ui->btnRunCode, &QPushButton::clicked, this, &MainWindow::btnRunCode_clicked);
    connect(ui->btnLoadCode, &QPushButton::clicked, this, &MainWindow::btnLoadCode_clicked);
    connect(ui->cmdLineEdit, &QLineEdit::returnPressed, this, &MainWindow::cmdLineEdit_return);
    connect(text, &Text::sendError, this, &MainWindow::errorInfo_print);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::cmdLineEdit_editingFinished()
{
    QString cmd = ui->cmdLineEdit->text();
    ui->cmdLineEdit->setText("");
    execCommand(cmd);
}

void MainWindow::btnClearCode_clicked()
{
    text->clear();
    ui->CodeDisplay->clear();
    ui->treeDisplay->clear();
    ui->textBrowser->clear();
}

void MainWindow::btnRunCode_clicked()
{
    ui->textBrowser->clear();
    Text::error = false;
    text->start();
}

void MainWindow::btnLoadCode_clicked()
{
    QString filePath = QFileDialog::getOpenFileName();
//    QString filePath = "../basicSrc/case3";
    parseFile(filePath);
}

void MainWindow::cmdLineEdit_return()
{
//    qDebug() << ui->cmdLineEdit->text();
}

void MainWindow::textBrowser_print(int value)
{
    auto stdPrintText = std::to_string(value);
    QString printText = QString(stdPrintText.c_str());
    ui->textBrowser->append(printText);
}

void MainWindow::parseFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString errorMsg = "cannot open file " + filePath;
        ui->textBrowser->append(errorMsg);
    }
    QTextStream in(&file);
    text->clear();
    auto allText = in.readAll();
    text->input(allText);
    auto &lines = Text::lines;
    for (auto &line: lines) {
        connectLine(line);
    }
    ui->CodeDisplay->clear();
    setCodeDisplayText();
    setTreeDisplayText();
}


void MainWindow::execCommand(QString &command)
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
    if (Text::waitForInput && splitCommand.first().first == "?") {
        int value = splitCommand.last().first.toInt();
        emit sendInputValue(value);
        return;
    }
    if (splitCommand.first().second == Token::Kind::Number) {
        if (splitCommand.size() == 1) {
            int removeLineNum = command.toInt();
            Text::lines.remove(removeLineNum);
        } else {
            int insertLineNum = splitCommand.first().first.toInt();
            Statement line(command);
            connectLine(line);
            Text::lines[insertLineNum] = Statement(command);
        }
        setCodeDisplayText();
        setTreeDisplayText();
        return;
    }
    if (command == "RUN") {
        execRun();
    } else if (command == "LOAD") {
        execLoad();
    } else if (command == "LIST") {
        execList();
    } else if (command == "CLEAR") {
        execClear();
    } else if (command == "HELP") {
        execHelp();
    } else if (command == "QUIT") {
        execQuit();
    } else {
        auto errorMsg = QString("Error command!") + command;
        errorInfo_print(errorMsg);
    }
}

void MainWindow::setCodeDisplayText()
{
    ui->CodeDisplay->clear();
    for (const auto &line: Text::lines)
        ui->CodeDisplay->append(line.rowLine);
}

void MainWindow::setTreeDisplayText()
{
    ui->treeDisplay->clear();
    for (auto &line: Text::lines)
        ui->treeDisplay->append(line.tree);
}

void MainWindow::execQuit()
{
    this->close();
}

void MainWindow::execRun()
{
    btnRunCode_clicked();
}

void MainWindow::execLoad()
{
    btnLoadCode_clicked();
}

void MainWindow::execList()
{

}

void MainWindow::execClear()
{
    btnClearCode_clicked();
}

void MainWindow::execHelp()
{
    QString help = "Hello world!";
    ui->textBrowser->append(help);
}

void MainWindow::cmdLineEdit_input()
{
    ui->cmdLineEdit->setText("?");
}

void MainWindow::errorInfo_print(QString error)
{
    ui->textBrowser->append(error);
}

void MainWindow::connectLine(Statement &line)
{
    if (line.type == PRINT)
        connect(&line, &Statement::textPrint, this, &MainWindow::textBrowser_print);
    else if (line.type == INPUT) {
        connect(&line, &Statement::prepareInput, this, &MainWindow::cmdLineEdit_input);
        connect(this, &MainWindow::sendInputValue, &line, &Statement::getInput);
    }
    connect(&line, &Statement::sendError, this, &MainWindow::errorInfo_print);

}
