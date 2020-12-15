#ifndef SERIAL_H
#define SERIAL_H
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QMutex>
struct Modbus_Flag
{
    const int Success = 1;
    const int ID_Err = 2;
    const int Func_Err = 3;
    const int CRC_Err = 4;
    const int Outtime = 5;
};

class Serial :public QObject
{
    Q_OBJECT
public:
    explicit Serial(QObject *parent = nullptr);
    ~Serial();
    void open_com();
    void close_com();
    void init();
    QSerialPort *serial = nullptr;
    unsigned int CRC16(unsigned char *ptr, unsigned int len);
    int Read_03_04_Reg(uint ID,uint Address,uint Len,uint reg);
    int Write_10_Reg(uint ID,uint Address,QByteArray& v);
signals:
    void Data_Updata_Signal(int);
    void Error_Tip_Signal(QString);
public slots:
    void doworks();
    void Stop_Thread();
public:
    int Scene_pos_pre = -1;
    int Scene_pos = -1;
    int cnt = 0;
    uint outtime_cnt = 0;
    QMutex m_mutex;
    QByteArray Read_Buf;
    QByteArray Read_Save_Buf;
    bool stopped = 0;
    bool SEND_03_04_FLAG;
    bool SEND_10_FLAG;
    bool START_RECEIVE_FLAG;
    bool RECEIVE_FINISH_FLAG;
    bool Write_FLAG;
    bool READING_FLAG;
};

#endif // SERIAL_H
