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


private slots:

    void cmdLineEdit_editingFinished();

    void btnClearCode_clicked();

    void btnRunCode_clicked();

    void btnLoadCode_clicked();

    void cmdLineEdit_return();

public slots:

    void textBrowser_print(int value);

    void setCodeDisplayText();

    void cmdLineEdit_input();

    void execQuit();

    void execRun();

    void execLoad();

    void execList();

    void execClear();

    void execHelp();

signals:

    void sendCommand(QString &command);


private:
    Ui::MainWindow *ui;
};

