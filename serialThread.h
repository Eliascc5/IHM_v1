#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QCoreApplication>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QThread>
#include <QMutex>

class SerialThread : public QThread
{
    Q_OBJECT

public:
    SerialThread(QObject *parent = nullptr)
        : QThread(parent)
    {   /*
        m_serial.setPortName("COM1");
        m_serial.setBaudRate(QSerialPort::Baud115200);
        m_serial.setDataBits(QSerialPort::Data8);
        m_serial.setParity(QSerialPort::NoParity);
        m_serial.setStopBits(QSerialPort::OneStop);
        m_serial.setFlowControl(QSerialPort::NoFlowControl);
        */

    }

    void setPort(QString aux){m_serial.setPortName(aux);}// Puerto serial a utilizar
    void setBaudRate(int aux){m_serial.setBaudRate(aux);}// Velocidad en baudios
    void setDataBits(QSerialPort::DataBits aux){m_serial.setDataBits(aux);}// Bits de datos
    void setParity(QSerialPort::Parity aux){m_serial.setParity(aux);}// Paridad
    void setStopBits(QSerialPort::StopBits aux){m_serial.setStopBits(aux);}// Bits de parada
    void setFlowControl(QSerialPort::FlowControl aux){m_serial.setFlowControl(aux);}// Control de flujoxºx

    QStringList  getAvailablePorts(){

        QList<QSerialPortInfo> ports = info.availablePorts();
        QList<QString> stringPorts;
        for(int i = 0 ; i < ports.size() ; i++){
            stringPorts.append(ports.at(i).portName());
        }
    return stringPorts;
    }
    QStringList  getAvailableBR(){

        QList<qint32> baudRates = info.standardBaudRates();
        QList<QString> stringBaudRates;
        for(int i = 0 ; i < baudRates.size() ; i++){
            stringBaudRates.append(QString::number(baudRates.at(i)));
        }
        return stringBaudRates;
    }

    bool getFlag_Open_Close(void){return serialPortOpened;}

    void writeData(const QByteArray &datos) {

        m_serial.write(datos);
    }


    ~SerialThread() override
    {

        wait();
    }

signals:
    void dataReceived(const QByteArray &data);
    void stateChanged(QString label);

public slots:

    void stopThread()
    {
        m_mutex.lock();
        m_quit = true;
        m_mutex.unlock();

    };

protected:
    void run() override
    {
        m_mutex.lock();
        m_quit = false;
        m_mutex.unlock();

        if (!m_serial.open(QIODevice::ReadWrite)) {
            serialPortOpened = false;
            emit stateChanged("Error");
            //qDebug() << "Error al abrir el puerto serial";
            return;
        }else{
            serialPortOpened = true;
            emit stateChanged("Connected");
            }

        while (true) {
            if (m_quit) {
                serialPortOpened = false;
                emit stateChanged("Disconnected");
                break;
            }
            if (m_serial.waitForReadyRead(1000)) { // Esperar TRES segundo a que lleguen datos por el puerto serial
                QByteArray data = m_serial.readAll(); // Leer todos los datos disponibles en el puerto serial
                emit dataReceived(data); // Emitir señal con los datos recibidos
            }
        }

        m_serial.close();
    }


private:
    QSerialPort m_serial;
    QSerialPortInfo info;
    QMutex m_mutex;
    bool m_quit;
    bool serialPortOpened;
};


#endif // SERIALTHREAD_H
