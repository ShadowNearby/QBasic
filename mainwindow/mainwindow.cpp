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
    connect(this, &MainWindow::sendCommand, text, &Text::executeCommand);
    connect(text, &Text::resetCodeText, this, &MainWindow::setCodeDisplayText);
    connect(text, &Text::sendClear, this, &MainWindow::execClear);
    connect(text, &Text::sendHelp, this, &MainWindow::execHelp);
    connect(text, &Text::sendList, this, &MainWindow::execList);
    connect(text, &Text::sendLoad, this, &MainWindow::execLoad);
    connect(text, &Text::sendQuit, this, &MainWindow::execQuit);
    connect(text, &Text::sendRun, this, &MainWindow::execRun);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::cmdLineEdit_editingFinished()
{
    QString cmd = ui->cmdLineEdit->text();
    ui->cmdLineEdit->setText("");
    emit sendCommand(cmd);
}

void MainWindow::btnClearCode_clicked()
{
    Text::clear();
    ui->CodeDisplay->clear();
    ui->treeDisplay->clear();
    ui->textBrowser->clear();
}

void MainWindow::btnRunCode_clicked()
{
    ui->textBrowser->clear();
    text->start();
}

void MainWindow::btnLoadCode_clicked()
{
//    QString filePath = QFileDialog::getOpenFileName();
    QString filePath = "../basicSrc/case3";
    parseFile(filePath);
}

void MainWindow::cmdLineEdit_return()
{
    qDebug() << ui->cmdLineEdit->text();
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
        qDebug() << "cannot open file " + filePath;
    }
    QTextStream in(&file);
    Text::clear();
    auto allText = in.readAll();
    text->input(allText);
    auto &lines = Text::lines;
    for (auto &line: lines) {
        if (line.type == PRINT)
            connect(&line, &Statement::textPrint, this, &MainWindow::textBrowser_print);
        else if (line.type == INPUT) {
            connect(text, &Text::sendInputValue, &line, &Statement::getInput);
            connect(&line, &Statement::prepareInput, this, &MainWindow::cmdLineEdit_input);
        }
    }
    ui->CodeDisplay->clear();
    setCodeDisplayText();
}

void MainWindow::setCodeDisplayText()
{
    ui->CodeDisplay->clear();
    for (const auto &line: Text::lines)
        ui->CodeDisplay->append(line.rowLine);
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
    qDebug() << "no help";
}

void MainWindow::cmdLineEdit_input()
{
    ui->cmdLineEdit->setText("? ");
}




