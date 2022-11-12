#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    explicit MainWindow(QWidget *parent = nullptr);


    ~MainWindow() override;

private slots:

    void cmdLineEdit_editingFinished();

    void btnClearCode_clicked();

    void btnRunCode_clicked();

    void btnLoadCode_clicked();

    void cmdLineEdit_return();

private:
    Ui::MainWindow *ui;
    QThread *mainThread;
};

#endif // MAINWINDOW_H
