#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->cmdLineEdit, &QLineEdit::editingFinished, this, &MainWindow::cmdLineEdit_editingFinished);
    connect(ui->btnClearCode, &QPushButton::clicked, this, &MainWindow::btnClearCode_clicked);
    connect(ui->btnRunCode, &QPushButton::clicked, this, &MainWindow::btnRunCode_clicked);
    connect(ui->btnLoadCode, &QPushButton::clicked, this, &MainWindow::btnLoadCode_clicked);
    connect(ui->cmdLineEdit, &QLineEdit::returnPressed, this, &MainWindow::cmdLineEdit_return);
    test();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::cmdLineEdit_editingFinished()
{
    QString cmd = ui->cmdLineEdit->text();
    ui->cmdLineEdit->setText("");

    ui->CodeDisplay->append(cmd);
}

void MainWindow::btnClearCode_clicked()
{
    qDebug() << ui->btnClearCode->text();
    ui->CodeDisplay->clear();
    ui->treeDisplay->clear();
    ui->textBrowser->clear();
}

void MainWindow::btnRunCode_clicked()
{
    qDebug() << ui->btnRunCode->text();

}

void MainWindow::btnLoadCode_clicked()
{
//    qDebug() << ui->btnLoadCode->text();
}

void MainWindow::cmdLineEdit_return()
{
    qDebug() << ui->cmdLineEdit->text();
}
