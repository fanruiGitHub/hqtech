#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QThread>

#include "netmanager.h"
#include "smmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_button1_clicked();
    void readoutput();
    void readdstandoutput();
    void readerrooutput();
    void on_button1_2_clicked();

    void on_reboot_button_clicked();

private:
    Ui::MainWindow *ui;

    QProcess *terminal;
    QThread nmThread, smThread;
    NetManager* netManager;
    SmManager* smManager;

public slots:
    void handleNetManagerResult(const QString &);

signals:
    void startNetManager();
    void startSmManager();

};

#endif // MAINWINDOW_H
