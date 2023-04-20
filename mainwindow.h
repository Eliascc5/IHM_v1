#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <string>
#include <iostream>
#include <cstdlib>
#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QCoreApplication>
#include "serialThread.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define NUMBEROFPOINTS 100


extern float jmax,jmin;
extern float vmax,vmin;
extern float vi,vf;
extern float amax,amin;
extern float T,Ta,Td,Tv,Tj1,Tj2,Tj,delta;
extern float qi,qf;
extern float q,qd,qdd,qddd;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_connectButton_clicked();

    void receiveMessage(const QByteArray &dataFromMCU);

    void updateLabel(QString newState);

    void on_sendTrameButton_clicked();

    void on_clearTextBrowserButton_clicked();

    void on_homingButton_clicked();

    void on_xpButton_clicked();

    void on_xnButton_clicked();

    void on_ypButton_clicked();

    void on_ynButton_clicked();

    void on_zpButton_clicked();

    void on_znButton_clicked();

    void on_plotButton_clicked();



    void on_openFileButton_clicked();

    void on_quitButton_clicked();

    void on_sendFileButton_clicked();

    void on_disconnectButton_clicked();

private:

    Ui::MainWindow *ui;

    SerialThread m_serialThread; // Instancia de SerialThread
    QByteArray dataFromMCU;
    bool portOpened;
    QString traetedMessage;
    int counterLines;

    QString buffer;
    QString code;
    QString fileName;
    QString pointXYZ[NUMBEROFPOINTS];
    int codeSize;
    bool FlagHoming,fileOpened;
    std::string ss;
    std::string aux;


};
#endif // MAINWINDOW_H
