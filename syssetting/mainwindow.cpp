#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->plainTextEdit->verticalScrollBar()->setStyleSheet("QScrollBar:vertical"
                                                          "{"
                                                          "width:40px;"
                                                          "background:rgba(0,0,0,0%);"
                                                          "margin:0px,0px,0px,0px;"
                                                          "padding-top:9px;"
                                                          "padding-bottom:9px;"
                                                          "}"
                                                          "QScrollBar::handle:vertical"
                                                          "{"
                                                          "width:40px;"
                                                          "background:rgba(0,0,0,75%);"
                                                          " border-radius:4px;"
                                                          "min-height:20;"
                                                          "}"
                                                          );

    terminal = new QProcess;
    connect(terminal,SIGNAL(readyRead()),this,SLOT(readoutput()));
    connect(terminal,SIGNAL(readyReadStandardOutput()),this, SLOT(readdstandoutput()));
    connect(terminal,SIGNAL(readyReadStandardError()),this, SLOT(readerrooutput()));

    QString RunFrameNcFile = "/etc/weston.ini";
    QFile Ncfile(RunFrameNcFile);
    Ncfile.open(QIODevice::ReadOnly);

    if (Ncfile.isOpen())
    {
        QTextStream NctextStream(&Ncfile);
        QString strTemp, allTemp;
        bool modified = false;
        while(!NctextStream.atEnd())
        {
            strTemp = NctextStream.readLine();
            if(strTemp.mid(0,5) == "mode=")
            {
                if(strTemp.mid(5, strTemp.length() - 5) != "1920x1080")
                {
                    allTemp += "mode=1920x1080";
                    allTemp += QString('\n').toLatin1();
                    modified = true;
                }
                else
                {
                    allTemp += strTemp;
                    allTemp += QString('\n').toLatin1();
                }
            }
            else
            {
                allTemp += strTemp;
                allTemp += QString('\n').toLatin1();
            }
        }

        ui->plainTextEdit->setPlainText(allTemp);

        Ncfile.close();

        if(modified)
        {
            Ncfile.open(QIODevice::WriteOnly);
            QTextStream in(&Ncfile);
            in << allTemp;
            terminal->start("reboot");
        }

    }

    terminal->start("ifconfig eth1 192.168.1.91 netmask 255.255.255.0");
    terminal->waitForFinished();
    terminal->start("route add -net 192.168.1.0 netmask 255.255.255.0 gw 0.0.0.0 dev eth1");
    terminal->waitForFinished();
    terminal->start("route add -net 0.0.0.0 netmask 0.0.0.0 gw 192.168.1.1 dev eth1");
    terminal->waitForFinished();

    netManager = new NetManager();
    netManager->moveToThread(&nmThread);
    connect(&nmThread,&QThread::finished, netManager, &QObject::deleteLater);
    connect(this, SIGNAL(startNetManager()), netManager, SLOT(doWork()));
    connect(netManager, &NetManager::resultReady, this, &MainWindow::handleNetManagerResult, Qt::QueuedConnection);
    nmThread.start();
    emit startNetManager();

    smManager = new SmManager();
    smManager->moveToThread(&smThread);
    connect(&smThread,&QThread::finished, smManager, &QObject::deleteLater);
    connect(this, SIGNAL(startSmManager()), smManager, SLOT(sendData()));
    connect(smManager, &SmManager::resultReady, this, &MainWindow::handleNetManagerResult, Qt::QueuedConnection);
    smThread.start();
    emit startSmManager();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button1_clicked()
{
    //terminal->waitForFinished();
    //terminal->start("ping -c 4 192.168.0.106");
    //terminal->waitForFinished();
    //terminal->start("ifconfig eth1 down");
    //terminal->waitForFinished();
}

void MainWindow::readoutput()
{
    if(ui->plainTextEdit->blockCount()>500)            //文本框中行数超过500行则清空
             ui->plainTextEdit->setPlainText("");
    ui->plainTextEdit->appendPlainText(terminal->readAll());  //在文本框中添加读到的输出信息
}

void MainWindow::readdstandoutput()
{
    ui->plainTextEdit->appendPlainText(terminal->readAllStandardOutput());
}

void MainWindow::readerrooutput()
{
    ui->plainTextEdit->appendPlainText(terminal->readAllStandardError());
}

void MainWindow::on_button1_2_clicked()
{
    terminal->start("ifconfig eth1 192.168.1.91 netmask 255.255.255.0");
    terminal->waitForFinished();
    terminal->start("route add -net 192.168.1.0 netmask 255.255.255.0 gw 0.0.0.0 dev eth1");
    terminal->waitForFinished();
    terminal->start("route add -net 0.0.0.0 netmask 0.0.0.0 gw 192.168.1.1 dev eth1");
    terminal->waitForFinished();
    terminal->start("ifconfig eth1 up");
    terminal->waitForFinished();
    terminal->start("ping -c 4 192.168.1.91");
    terminal->waitForFinished();

    QByteArray qbaOutput = terminal->readAllStandardOutput();
    QString qstrYouWant = qbaOutput;
    ui->plainTextEdit->appendPlainText(qstrYouWant);
}

void MainWindow::handleNetManagerResult(const QString &result)
{
    if(ui->plainTextEdit->blockCount()>15)            //文本框中行数超过500行则清空
        ui->plainTextEdit->setPlainText("");
    ui->plainTextEdit->appendPlainText(result);
}

void MainWindow::on_reboot_button_clicked()
{
    terminal->start("reboot");
}
