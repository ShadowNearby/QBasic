#pragma once

#include "../utility/QUtility.h"
#include "../parser/Lexer.h"
#include "../statement/Text.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    Text *text;

    explicit MainWindow(QWidget *parent = nullptr);

    void parseFile(const QString &filePath);

    ~MainWindow() override;

    void execCommand(QString &command);

    void connectLine(Statement &line);

private slots:

    void cmdLineEdit_editingFinished();

    void btnClearCode_clicked();

    void btnRunCode_clicked();

    void btnLoadCode_clicked();

    void cmdLineEdit_return();

public slots:

    void textBrowser_print(int value);

    void setCodeDisplayText();

    void setTreeDisplayText();

    void cmdLineEdit_input();

    void errorInfo_print(QString error);

    void execQuit();

    void execRun();

    void execLoad();

    void execList();

    void execClear();

    void execHelp();

signals:

    void sendInputValue(int value);

private:
    Ui::MainWindow *ui;
};

