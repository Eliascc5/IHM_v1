#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "doubleSprofile.h"
#include "GlobalFunc.h"


#define nbPointsTime 100

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Fixing size of main windows

    QMainWindow::setFixedSize(700, 650);

    connect(&m_serialThread, &SerialThread::dataReceived, this, &MainWindow::receiveMessage);
    connect(&m_serialThread, &SerialThread::stateChanged, this, &MainWindow::updateLabel);
    portOpened=false;
    // Ports

    QList<QString> stringPorts = m_serialThread.getAvailablePorts();
    ui->comboBox_port->addItems(stringPorts);

    QList<QString> stringBaudRates = m_serialThread.getAvailableBR();
    ui->comboBox_baud->addItems(stringBaudRates);

    /*Configuration of QCustomPlot*/

    //Create Rectangle Area and add to QCustomplot
    QCPAxisRect * R00=new QCPAxisRect(this->ui->velProfilePlot);
    QCPAxisRect * R10=new QCPAxisRect(this->ui->velProfilePlot);
    QCPAxisRect * R20=new QCPAxisRect(this->ui->velProfilePlot);
    QCPAxisRect * R30=new QCPAxisRect(this->ui->velProfilePlot);

    this->ui->velProfilePlot->plotLayout()->clear();
    this->ui->velProfilePlot->plotLayout()->addElement(0,0,R00);
    this->ui->velProfilePlot->plotLayout()->addElement(1,0,R10);
    this->ui->velProfilePlot->plotLayout()->addElement(2,0,R20);
    this->ui->velProfilePlot->plotLayout()->addElement(3,0,R30);

    //Draw graphs with data
    this->ui->velProfilePlot->addGraph(R00->axis(QCPAxis::atBottom),R00->axis(QCPAxis::atLeft));
    this->ui->velProfilePlot->addGraph(R10->axis(QCPAxis::atBottom),R10->axis(QCPAxis::atLeft));
    this->ui->velProfilePlot->addGraph(R20->axis(QCPAxis::atBottom),R20->axis(QCPAxis::atLeft));
    this->ui->velProfilePlot->addGraph(R30->axis(QCPAxis::atBottom),R30->axis(QCPAxis::atLeft));

    this->ui->velProfilePlot->graph(0)->setName("Position");
    this->ui->velProfilePlot->graph(1)->setName("Velocity");
    this->ui->velProfilePlot->graph(2)->setName("Aceleration");
    this->ui->velProfilePlot->graph(3)->setName("Jerk");

    //Add Legends
    QCPLegend *arLegend00=new QCPLegend;
    R00->insetLayout()->addElement(arLegend00,Qt::AlignTop|Qt::AlignRight);
    arLegend00->setLayer("legend");
    arLegend00->addItem(new QCPPlottableLegendItem(arLegend00, this->ui->velProfilePlot->graph(0)));

    QCPLegend *arLegend10=new QCPLegend;
    R10->insetLayout()->addElement(arLegend10,Qt::AlignTop|Qt::AlignRight);
    arLegend10->setLayer("legend");
    arLegend10->addItem(new QCPPlottableLegendItem(arLegend10, this->ui->velProfilePlot->graph(1)));

    QCPLegend *arLegend20=new QCPLegend;
    R20->insetLayout()->addElement(arLegend20,Qt::AlignTop|Qt::AlignRight);
    arLegend20->setLayer("legend");
    arLegend20->addItem(new QCPPlottableLegendItem(arLegend20, this->ui->velProfilePlot->graph(2)));

    QCPLegend *arLegend30=new QCPLegend;
    R30->insetLayout()->addElement(arLegend30,Qt::AlignTop|Qt::AlignRight);
    arLegend30->setLayer("legend");
    arLegend30->addItem(new QCPPlottableLegendItem(arLegend30, this->ui->velProfilePlot->graph(3)));

}

MainWindow::~MainWindow()
{
    m_serialThread.quit();
    m_serialThread.wait();
    delete ui;
}



void MainWindow::on_connectButton_clicked()
{

    QString portName = ui->comboBox_port->currentText();
    m_serialThread.setPort(portName);

    QString stringbaudRate = ui->comboBox_baud->currentText();
    int intbaudRate = stringbaudRate.toInt();
    m_serialThread.setBaudRate(intbaudRate);

    m_serialThread.setDataBits(QSerialPort::Data8);
    m_serialThread.setParity(QSerialPort::NoParity);
    m_serialThread.setStopBits(QSerialPort::OneStop);
    m_serialThread.setFlowControl(QSerialPort::NoFlowControl);

    m_serialThread.start(); //ejecuta la funcion run


}


void MainWindow::updateLabel(QString label)
{

    if(label =="Connected"){
        portOpened = m_serialThread.getFlag_Open_Close();
        ui->textBrowser->setTextColor(Qt::green);
        ui->textBrowser->append(label);
        code = "\n";
        codeSize = code.size();
    }else if (label=="Disconnected")
    {
        portOpened = m_serialThread.getFlag_Open_Close();
        ui->textBrowser->setTextColor(Qt::red);
        ui->textBrowser->append(label);
    }
    else {
        portOpened = m_serialThread.getFlag_Open_Close();
        ui->textBrowser->setTextColor(Qt::red);
        ui->textBrowser->append("Something went wrong, try again!");


    }
}


void MainWindow::receiveMessage(const QByteArray &dataFromMCU)
{
    //QByteArray dataBA = serialPort.readAll();
    QString data(dataFromMCU);

    buffer.append(data);
    int index = buffer.indexOf(code);
    if(index != -1){
       traetedMessage = buffer.mid(0,index);
       ui->textBrowser->setTextColor(Qt::blue); // Receieved message's color is blue.
       ui->textBrowser->append(traetedMessage);
       buffer.remove(0,index+codeSize);

       if (traetedMessage =="done"){
           MainWindow::on_sendFileButton_clicked();

       }
    }


}


void MainWindow::on_sendTrameButton_clicked()
{

    if(!portOpened){
        //TODO: AGREGAR QMESSAGEBOX CON WARNING
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("The serial port is NOT connected!");
        msgBox.exec();
    }
    else {
        QString msg = ui->lineEdit_Trame->text()+13; //13 en ascii es CR
        ui->textBrowser->setTextColor(Qt::darkGreen); // Color of message to send is green.
        ui->textBrowser->append(msg);
        qDebug()<<"---------";
        m_serialThread.writeData(msg.toUtf8());
    }

}



void MainWindow::on_clearTextBrowserButton_clicked()
{
     ui->textBrowser->clear();
}





void MainWindow::on_homingButton_clicked()
{
    if(!portOpened){
        //TODO: AGREGAR QMESSAGEBOX CON WARNING
        QMessageBox msgBox;
        msgBox.setText("The serial port is NOT connected!");
        msgBox.exec();
    }
    else {
       QString message = ":h\r";
       m_serialThread.writeData(message.toUtf8());
       FlagHoming = true;


    }
}


void MainWindow::on_xpButton_clicked()
{
    if(FlagHoming){

        //TODO: ENVIAR TRAMA PARA MOVERSE EN X+

        QString aux1{":px"};
        QString step{ui->lineEdit_step->text()};
        QString aux2 ={"\r"};

        QString command = aux1+step+aux2;

        qDebug()<<command;
        m_serialThread.writeData(command.toUtf8());
    }
    else{

        QMessageBox msgBox;
        msgBox.setText("First, you have to do a homing!");
        msgBox.exec();
    }
}


void MainWindow::on_xnButton_clicked()
{
    if(FlagHoming){
        //TODO: ENVIAR TRAMA PARA MOVERSE EN X-
        QString aux1{":px"};
        QString step{"-" + ui->lineEdit_step->text()};
        QString aux2 ={"\r"};

        QString command = aux1+step+aux2;

        qDebug()<<"aca abajo:\n";
        qDebug()<<command;
        m_serialThread.writeData(command.toUtf8());
    }
    else{

        QMessageBox msgBox;
        msgBox.setText("First, you have to do a homing!");
        msgBox.exec();
    }

}


void MainWindow::on_ypButton_clicked()
{
    if(FlagHoming){
        //TODO: ENVIAR TRAMA PARA MOVERSE EN Y+
        QString aux1{":py"};
        QString step{ui->lineEdit_step->text()};
        QString aux2 ={"\r"};

        QString command = aux1+step+aux2;

        qDebug()<<"aca abajo:\n";
        qDebug()<<command;
        m_serialThread.writeData(command.toUtf8());
    }
    else{

        QMessageBox msgBox;
        msgBox.setText("First, you have to do a homing!");
        msgBox.exec();
    }
}


void MainWindow::on_ynButton_clicked()
{
    if(FlagHoming){
        //TODO: ENVIAR TRAMA PARA MOVERSE EN Y-

        QString aux1{":py"};
        QString step{"-" + ui->lineEdit_step->text()};
        QString aux2 ={"\r"};

        QString command = aux1+step+aux2;

        qDebug()<<"aca abajo:\n";
        qDebug()<<command;
        m_serialThread.writeData(command.toUtf8());
    }
    else{

        QMessageBox msgBox;
        msgBox.setText("First, you have to do a homing!");
        msgBox.exec();
    }
}


void MainWindow::on_zpButton_clicked()
{
    if(FlagHoming){
        //TODO: ENVIAR TRAMA PARA MOVERSE EN Z+
        QString aux1{":pz"};
        QString step{ui->lineEdit_step->text()};
        QString aux2 ={"\r"};

        QString command = aux1+step+aux2;

        qDebug()<<"aca abajo:\n";
        qDebug()<<command;
        m_serialThread.writeData(command.toUtf8());
    }
    else{

        QMessageBox msgBox;
        msgBox.setText("First, you have to do a homing!");
        msgBox.exec();
    }
}


void MainWindow::on_znButton_clicked()
{
    if(FlagHoming){
        //TODO: ENVIAR TRAMA PARA MOVERSE EN Z-
        QString aux1{":pz"};
        QString step{"-" + ui->lineEdit_step->text()};
        QString aux2 ={"\r"};

        QString command = aux1+step+aux2;


        qDebug()<<command;
        m_serialThread.writeData(command.toUtf8());
    }
    else{

        QMessageBox msgBox;
        msgBox.setText("First, you have to do a homing!");
        msgBox.exec();
    }
}


void MainWindow::on_plotButton_clicked()
{


    vmax = (ui->lineEdit_vm->text()).toFloat();
    vi = (ui->lineEdit_vi->text()).toFloat();
    vf = (ui->lineEdit_vf->text()).toFloat();
    amax = (ui->lineEdit_am->text()).toFloat();
    jmax = (ui->lineEdit_jm->text()).toFloat();

    qf=(ui->lineEdit_qf->text()).toFloat();
    qi=(ui->lineEdit_qi->text()).toFloat();

    float *arrayT = update_ScurveTraj( qf , qi,  vi, vf , vmax, amax, jmax);

    for (int i=0;i<6;i++){
        qDebug()<<arrayT[i];
    }

    qDebug()<<"El valor de T es: "<<arrayT[5]<<"\n";

    float *time = linspace(0, arrayT[5], nbPointsTime);

    for (int i=0;i<nbPointsTime;i++){

        qDebug()<<"time: "<<time[i];
    }


    float *arrayCoord;
    float array_q[nbPointsTime],array_qd[nbPointsTime],array_qdd[nbPointsTime],array_qddd[nbPointsTime];


    for (int i=0;i<nbPointsTime;i++){

         arrayCoord = get_Straj(time[i]);
         array_q[i] = arrayCoord[0];
         array_qd[i] = arrayCoord[1];
         array_qdd[i] = arrayCoord[2];
         array_qddd[i] = arrayCoord[3];

    }

    QVector<double> x(nbPointsTime), y(nbPointsTime),y2(nbPointsTime),
            y3(nbPointsTime),y4(nbPointsTime);

    for (int i=0; i<nbPointsTime; i++)
    {
      x[i] = time[i];
      y[i] = array_q[i];
      y2[i] = array_qd[i];
      y3[i] = array_qdd[i];
      y4[i] = array_qddd[i];
    }


    this->ui->velProfilePlot->graph(0)->setData(x,y);
    this->ui->velProfilePlot->graph(0)->rescaleAxes();

    this->ui->velProfilePlot->graph(1)->setData(x,y2);
    this->ui->velProfilePlot->graph(1)->rescaleAxes();

    this->ui->velProfilePlot->graph(2)->setData(x,y3);
    this->ui->velProfilePlot->graph(2)->rescaleAxes();

    this->ui->velProfilePlot->graph(3)->setData(x,y4);
    this->ui->velProfilePlot->graph(3)->rescaleAxes();




    this->ui->velProfilePlot->replot();

    free(time);
    free(arrayT);
    free(arrayCoord);
}





void MainWindow::on_openFileButton_clicked()
{
    for (int i=0; i<NUMBEROFPOINTS;i++ ){ //para limpiar el buffer cada vez que se abre un archivo
        pointXYZ[i]="0";
    }

    // Abrir un cuadro de diálogo de selección de archivo
    fileName = QFileDialog::getOpenFileName(nullptr, "Abrir archivo", QString(), "Archivos de texto (*.txt)");
    if (fileName.isEmpty())
    {
        this->ui->textBrowser->append("empty filename");
    }

    // Abrir el archivo
    QFile inputFile(fileName);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->textBrowser->setTextColor(Qt::black);
        this->ui->textBrowser->append("Could not open the file");
        fileOpened=false;
    }
    else {

        fileOpened=true;
        ui->textBrowser->setTextColor(Qt::black);
        this->ui->textBrowser->append("the file was opened successfully");
        counterLines=0;
        ui->dirFile->setText(fileName);

        QTextStream in(&inputFile);
        int i = 0;
        while (!in.atEnd())
        {
            QString line = in.readLine();
            ui->textBrowser->setTextColor(Qt::black);
            pointXYZ[i]=line;
            i++;
        }

        // Cerrar el archivo
        inputFile.close();
    }



}

void MainWindow::on_sendFileButton_clicked()
{
    if(fileOpened && portOpened){
        qDebug()<<"e"<<fileOpened;
        qDebug()<<"e1"<<portOpened;
        m_serialThread.writeData((pointXYZ[counterLines]+"\r").toUtf8());
        this->ui->textBrowser->append(pointXYZ[counterLines]);
        if (pointXYZ[counterLines]=="0\n"){
            counterLines=0;
            this->ui->textBrowser->append("EOF reached.\n");
        }else{counterLines++;}
    }else{

        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Something went wrong. Please verify if the file has been opened and if the port is connected");
        msgBox.exec();

    }



}



void MainWindow::on_quitButton_clicked()
{
    m_serialThread.stopThread();

    this->close();
}

void MainWindow::on_disconnectButton_clicked()
{
    m_serialThread.stopThread();
}

