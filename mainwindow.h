#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include "toolbar/drivertoolbar.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLabel *byteLabel;
    QLabel *spinnerLabel;
    QTimer *statusTimer;
    void start();
    void welcome();
    void statusBarAnimation(DriverToolbar *dt);
    void clearText();
    void startTimer();
};
#endif // MAINWINDOW_H
