#include "serial.h"
#include "mainwindow.h"
#include "logindialog.h"
#include <windows.h>

uint MainWindow::addr;
uint MainWindow::outtime;
QString MainWindow::com;
int MainWindow::Baud;
QString MainWindow::DbName;
QList<uint> MainWindow::scene_items_num_list;  //每个画面的内容数（画面不重复）
QList<Signal_Info> MainWindow::Signal_info_list;
Signal_Info MainWindow::Clicked_Signal;
QByteArray MainWindow::Write_Value;
bool LoginDialog::LowInTheFirst;
Serial::Serial(QObject *parent) : QObject(parent)
{
    init();
}

Serial::~Serial()
{
    if(serial != nullptr)
        close_com();
    this->deleteLater();
}

void Serial::open_com()
{
    if(serial != nullptr)
    {
        serial->close();
        serial = nullptr;

    }
    if(serial == nullptr)
    {
        serial = new QSerialPort;
        serial->setPortName(MainWindow::com);
        serial->open(QIODevice::ReadWrite);
        serial->setBaudRate(MainWindow::Baud);//设置波特率
        serial->setDataBits(QSerialPort::Data8);//设置数据位8
        serial->setParity(QSerialPort::NoParity); //校验位设置为0
        serial->setStopBits(QSerialPort::OneStop);//停止位设置为1
        serial->setFlowControl(QSerialPort::NoFlowControl);//设置为无流控制
    }

}

void Serial::close_com()
{
    //关闭串口
    serial->clear();
    serial->close();
    serial->deleteLater();
    qDebug()<<"串口关闭"<<endl;

}

void Serial::init()
{
    SEND_03_04_FLAG = 0;
    SEND_10_FLAG = 0;
    RECEIVE_FINISH_FLAG = 0;
    START_RECEIVE_FLAG = 0;
    Write_FLAG = 0;
    READING_FLAG = 0;
}

void Serial::doworks()
{
    if(serial == nullptr)
        return;
    stopped = 0;
    if(serial->isOpen())
    {
        emit Data_Updata_Signal(-3);   //串口连接
        while (1)
        {
            QMutexLocker locker(&m_mutex);
            if(stopped)
            {
                return;
            }
            if((Write_FLAG == 1)&&(SEND_03_04_FLAG == 0))  //写值优先
            {
                Write_10_Reg(MainWindow::addr,MainWindow::Clicked_Signal.address,MainWindow::Write_Value);
            }
            else
            {
                if(Scene_pos != -1)
                {
                    if(Scene_pos != Scene_pos_pre)    //切换页面时保证串口写复位
                    {
                        Scene_pos_pre = Scene_pos;
                        serial->readAll();
                        SEND_03_04_FLAG = 0;
                        Read_Buf.clear();
                        Read_Save_Buf.clear();
                        START_RECEIVE_FLAG = 0;
                        RECEIVE_FINISH_FLAG = 0;
                        outtime_cnt = 0;

                    }
                    int pos = 0;             
                    for(int i = 0; i < Scene_pos; i++)
                    {
                        pos += MainWindow::scene_items_num_list[i];
                    }                  
                    switch (MainWindow::Signal_info_list.at(pos+cnt).data_type)   //根据数据类型确定长度
                    {
                        case 0:   //short
                        {
                            if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,1,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                            {
                                short temp;
                                if(LoginDialog::LowInTheFirst)
                                {
                                     temp = short((Read_Save_Buf[4]<<8)&0xff00)|(Read_Save_Buf[3]&0xff);
                                }
                                else
                                {
                                     temp = short((Read_Save_Buf[3]<<8)&0xff00)|(Read_Save_Buf[4]&0xff);
                                }

                                MainWindow::Signal_info_list[pos+cnt].Value = temp* MainWindow::Signal_info_list.at(pos+cnt).factor;
                                Read_Save_Buf.clear();
                                cnt++;
                                emit Data_Updata_Signal(cnt);
                            }
                        }
                        break;
                        case 1:   //ushort
                        {
                            if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,1,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                            {
                                ushort temp;
                                if(LoginDialog::LowInTheFirst)
                                {
                                     temp = ushort((Read_Save_Buf[4]<<8)&0xff00)|(Read_Save_Buf[3]&0xff);
                                }
                                else
                                {
                                     temp = ushort((Read_Save_Buf[3]<<8)&0xff00)|(Read_Save_Buf[4]&0xff);
                                }

                                MainWindow::Signal_info_list[pos+cnt].Value = temp* MainWindow::Signal_info_list.at(pos+cnt).factor;
                                Read_Save_Buf.clear();
                                cnt++;
                                emit Data_Updata_Signal(cnt);
                            }
                        }
                        break;
                        case 2:  //int(abcd)
                        {
                            if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,2,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                            {
                                int temp = static_cast<int>(((Read_Save_Buf[3]<<24)&0xff000000)|((Read_Save_Buf[4]<<16)&0xff0000)|((Read_Save_Buf[5]<<8)&0xff00)|(Read_Save_Buf[6]&0xff));
                                MainWindow::Signal_info_list[pos+cnt].Value = temp* MainWindow::Signal_info_list.at(pos+cnt).factor;
                                Read_Save_Buf.clear();
                                cnt++;
                                emit Data_Updata_Signal(cnt);
                            }
                        }
                        break;
                        case 3:  //int(cdab)
                        {
                            if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,2,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                            {
                                int temp = static_cast<int>(((Read_Save_Buf[5]<<24)&0xff000000)|((Read_Save_Buf[6]<<16)&0xff0000)|((Read_Save_Buf[3]<<8)&0xff00)|(Read_Save_Buf[4]&0xff));
                                MainWindow::Signal_info_list[pos+cnt].Value = temp* MainWindow::Signal_info_list.at(pos+cnt).factor;
                                Read_Save_Buf.clear();
                                cnt++;
                                emit Data_Updata_Signal(cnt);
                            }
                        }
                        break;
                        case 4:  //int(badc)
                        {
                            if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,2,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                            {
                                int temp = static_cast<int>(((Read_Save_Buf[4]<<24)&0xff000000)|((Read_Save_Buf[3]<<16)&0xff0000)|((Read_Save_Buf[6]<<8)&0xff00)|(Read_Save_Buf[5]&0xff));
                                MainWindow::Signal_info_list[pos+cnt].Value = temp* MainWindow::Signal_info_list.at(pos+cnt).factor;
                                Read_Save_Buf.clear();
                                cnt++;
                                emit Data_Updata_Signal(cnt);
                            }
                        }
                        break;
                        case 5:  //int(dcba)
                        {
                            if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,2,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                            {
                                int temp = static_cast<int>(((Read_Save_Buf[6]<<24)&0xff000000)|((Read_Save_Buf[5]<<16)&0xff0000)|((Read_Save_Buf[4]<<8)&0xff00)|(Read_Save_Buf[3]&0xff));
                                MainWindow::Signal_info_list[pos+cnt].Value = temp* MainWindow::Signal_info_list.at(pos+cnt).factor;
                                Read_Save_Buf.clear();
                                cnt++;
                                emit Data_Updata_Signal(cnt);
                            }
                        }
                        break;
                        case 6:  //uint(abcd)
                        {
                            if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,2,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                            {
                                uint temp = ((Read_Save_Buf[3]<<24)&0xff000000)|((Read_Save_Buf[4]<<16)&0xff0000)|((Read_Save_Buf[5]<<8)&0xff00)|(Read_Save_Buf[6]&0xff);
                                MainWindow::Signal_info_list[pos+cnt].Value = temp* MainWindow::Signal_info_list.at(pos+cnt).factor;
                                Read_Save_Buf.clear();
                                cnt++;
                                emit Data_Updata_Signal(cnt);
                            }
                        }
                        break;
                        case 7:  //uint(cdab)
                        {
                            if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,2,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                            {
                                uint temp = ((Read_Save_Buf[5]<<24)&0xff000000)|((Read_Save_Buf[6]<<16)&0xff0000)|((Read_Save_Buf[3]<<8)&0xff00)|(Read_Save_Buf[4]&0xff);
                                MainWindow::Signal_info_list[pos+cnt].Value = temp* MainWindow::Signal_info_list.at(pos+cnt).factor;
                                Read_Save_Buf.clear();
                                cnt++;
                                emit Data_Updata_Signal(cnt);
                            }
                        }
                        break;
                        case 8:  //uint(badc)
                        {
                            if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,2,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                            {
                                uint temp = ((Read_Save_Buf[4]<<24)&0xff000000)|((Read_Save_Buf[3]<<16)&0xff0000)|((Read_Save_Buf[6]<<8)&0xff00)|(Read_Save_Buf[5]&0xff);
                                MainWindow::Signal_info_list[pos+cnt].Value = temp* MainWindow::Signal_info_list.at(pos+cnt).factor;
                                Read_Save_Buf.clear();
                                cnt++;
                                emit Data_Updata_Signal(cnt);
                            }
                        }
                        break;
                        case 9:  //uint(dcba)
                        {
                            if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,2,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                            {
                                uint temp = ((Read_Save_Buf[6]<<24)&0xff000000)|((Read_Save_Buf[5]<<16)&0xff0000)|((Read_Save_Buf[4]<<8)&0xff00)|(Read_Save_Buf[3]&0xff);
                                MainWindow::Signal_info_list[pos+cnt].Value = temp* MainWindow::Signal_info_list.at(pos+cnt).factor;
                                Read_Save_Buf.clear();
                                cnt++;
                                emit Data_Updata_Signal(cnt);
                            }
                        }
                        break;
                        case 10:  //float(abcd)
                        {
                            if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,2,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                            {
                                float temp = ((Read_Save_Buf[3]<<24)&0xff000000)|((Read_Save_Buf[4]<<16)&0xff0000)|((Read_Save_Buf[5]<<8)&0xff00)|(Read_Save_Buf[6]&0xff);
                                MainWindow::Signal_info_list[pos+cnt].Value = temp* MainWindow::Signal_info_list.at(pos+cnt).factor;
                                Read_Save_Buf.clear();
                                cnt++;
                                emit Data_Updata_Signal(cnt);
                            }
                        }
                        break;
                        case 11:  //float(cdab)
                        {
                            if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,2,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                            {
                                float temp = ((Read_Save_Buf[5]<<24)&0xff000000)|((Read_Save_Buf[6]<<16)&0xff0000)|((Read_Save_Buf[3]<<8)&0xff00)|(Read_Save_Buf[4]&0xff);
                                MainWindow::Signal_info_list[pos+cnt].Value = temp* MainWindow::Signal_info_list.at(pos+cnt).factor;
                                Read_Save_Buf.clear();
                                cnt++;
                                emit Data_Updata_Signal(cnt);
                            }
                        }
                        break;
                        case 12:  //float(badc)
                        {
                            if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,2,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                            {
                                float temp = ((Read_Save_Buf[4]<<24)&0xff000000)|((Read_Save_Buf[3]<<16)&0xff0000)|((Read_Save_Buf[6]<<8)&0xff00)|(Read_Save_Buf[5]&0xff);
                                MainWindow::Signal_info_list[pos+cnt].Value = temp* MainWindow::Signal_info_list.at(pos+cnt).factor;
                                Read_Save_Buf.clear();
                                cnt++;
                                emit Data_Updata_Signal(cnt);
                            }
                        }
                        break;
                        case 13:  //float(dcba)
                        {
                            if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,2,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                            {
                                float temp = ((Read_Save_Buf[6]<<24)&0xff000000)|((Read_Save_Buf[5]<<16)&0xff0000)|((Read_Save_Buf[4]<<8)&0xff00)|(Read_Save_Buf[3]&0xff);
                                MainWindow::Signal_info_list[pos+cnt].Value = temp* MainWindow::Signal_info_list.at(pos+cnt).factor;
                                Read_Save_Buf.clear();
                                cnt++;
                                emit Data_Updata_Signal(cnt);
                            }
                        }
                        break;
                        case 14:   //string
                        {
                            if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,MainWindow::Signal_info_list.at(pos+cnt).offset,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                            {
                                QString temp = Read_Save_Buf.mid(3,MainWindow::Signal_info_list.at(pos+cnt).offset*2);
                                MainWindow::Signal_info_list[pos+cnt].Value = temp;
                                Read_Save_Buf.clear();
                                cnt++;
                                emit Data_Updata_Signal(cnt);
                            }
                        }
                        break;
                        case 15:   //datetime(abcdef)
                        {
                            if(LoginDialog::LowInTheFirst)//6寄存器格式的低字节版本
                            {
                                if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,6,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                                {
                                    QString temp = QString::number(Read_Save_Buf.at(3)+2000)+"."+
                                                        QString::number(Read_Save_Buf.at(5))+"."+
                                                        QString::number(Read_Save_Buf.at(7))+" "+
                                                        QString::number(Read_Save_Buf.at(9))+":"+
                                                        QString::number(Read_Save_Buf.at(11))+":"+
                                                        QString::number(Read_Save_Buf.at(13));
                                    MainWindow::Signal_info_list[pos+cnt].Value = temp;
                                    Read_Save_Buf.clear();
                                    cnt++;
                                    emit Data_Updata_Signal(cnt);
                                }
                            }
                            else//3寄存器格式
                            {
                                if(Read_03_04_Reg(MainWindow::addr,MainWindow::Signal_info_list.at(pos+cnt).address,3,MainWindow::Signal_info_list.at(pos+cnt).reg) == 1)
                                {
                                    QString temp = QString::number(Read_Save_Buf.at(3)+2000)+"."+
                                                        QString::number(Read_Save_Buf.at(4))+"."+
                                                        QString::number(Read_Save_Buf.at(5))+" "+
                                                        QString::number(Read_Save_Buf.at(6))+":"+
                                                        QString::number(Read_Save_Buf.at(7))+":"+
                                                        QString::number(Read_Save_Buf.at(8));
                                    MainWindow::Signal_info_list[pos+cnt].Value = temp;
                                    Read_Save_Buf.clear();
                                    cnt++;
                                    emit Data_Updata_Signal(cnt);
                                }
                            }

                        }
                        break;
                    }
                    if(cnt >= MainWindow::scene_items_num_list[Scene_pos])
                        cnt = 0;
                }
            }
        }
    }
    else
    {
        emit Data_Updata_Signal(-4);   //串口无法打开
        qDebug()<<"串口未打开";
    }
}

void Serial::Stop_Thread()
{
    init();
    stopped = 1;
    qDebug()<<"thread stop";
}

unsigned int Serial::CRC16(unsigned char *ptr, unsigned int len)
{
    unsigned char j, CRClen;
    unsigned int CRCdata;
    CRCdata = 0xffff;
    for(CRClen = 0; CRClen < len; CRClen++)
    {
        CRCdata^= ptr[CRClen];
        for(j = 0; j < 8; ++j)
        {
            if(CRCdata & 0x01)
            {
                CRCdata = (CRCdata >> 1)^0xa001;
            }
            else
            {
                CRCdata = (CRCdata >> 1);
            }
        }
    }
    CRCdata = ((CRCdata & 0xff) << 8)|(CRCdata >> 8);
    return CRCdata;
}

int Serial::Read_03_04_Reg(uint ID,uint Address,uint Len,uint reg)
{
    QByteArray SEND_Buf;
    Modbus_Flag Flag;
    if(SEND_03_04_FLAG == 0)                //询问指令未发送
    {
        SEND_Buf[0] = char(ID);
        SEND_Buf[1] = char(reg&0xff);
        SEND_Buf[2] = char(Address>>8);
        SEND_Buf[3] = char(Address&0xff);
        SEND_Buf[4] = char(Len>>8);
        SEND_Buf[5] = char(Len&0xff);
        unsigned char temp_buf[10];
        memcpy(temp_buf,SEND_Buf,6);
        SEND_Buf[6] = char(CRC16(temp_buf,6)>>8);
        SEND_Buf[7] = char(CRC16(temp_buf,6)&0xff);
        serial->write(SEND_Buf.constData(),8);
        SEND_03_04_FLAG = 1;
        Read_Buf.clear();
        START_RECEIVE_FLAG = 0;
        RECEIVE_FINISH_FLAG = 0;
        outtime_cnt = 0;
    }
    else                                      //询问指令发送完成，等待接收
    {
        if(serial->waitForReadyRead(40))   //40ms未接收到下一帧数据，即本帧数据接收完成
        {
            if(RECEIVE_FINISH_FLAG == 0)
            {
                START_RECEIVE_FLAG = 1;
                Read_Buf += serial->readAll();
            }
        }
        else      //超时未接收到回复数据
        {
            if(START_RECEIVE_FLAG == 1)
            {
                RECEIVE_FINISH_FLAG = 1;
                if(RECEIVE_FINISH_FLAG == 1)
                {
                    if(Read_Buf.length() >= 5)
                    {
                        if(Read_Buf.at(0) != char(ID))
                        {
                            emit Error_Tip_Signal(tr("Slave ID Error"));
                            emit Data_Updata_Signal(-2);   //通讯异常
                            //qDebug()<<"Flag.ID_Err";
                            SEND_03_04_FLAG = 0;
                            return Flag.ID_Err;
                        }
                        if(Read_Buf.at(1) != uchar(reg&0xff))
                        {
                            if(Read_Buf.at(1) == uchar(reg|0x80))
                            {
                                uint val = Read_Buf.at(2);
                                emit Error_Tip_Signal(tr("Error Num:%1").arg(val));
                            }
                            emit Data_Updata_Signal(-2);   //通讯异常
                            //qDebug()<<"Flag.Func_Err";
                            SEND_03_04_FLAG = 0;
                            return Flag.Func_Err;
                        }
                        int len = Read_Buf.length();
                        unsigned char temp_buf[100];
                        memcpy(temp_buf,Read_Buf,uint(len-2));
                        uint CRC = CRC16(temp_buf,uint(len-2));
                        if((Read_Buf.at(len-2) == char(CRC>>8))&&(Read_Buf.at(len-1) == char(CRC&0xff)))  //接收完成
                        {
                            emit Error_Tip_Signal(tr(""));
                            //qDebug()<<"Flag.Success";
                            SEND_03_04_FLAG = 0;
                            Read_Save_Buf = Read_Buf;
                            return Flag.Success;
                        }
                        else
                        {
                            emit Data_Updata_Signal(-2);   //通讯异常
                            //qDebug()<<"Flag.CRC_Err";
                            SEND_03_04_FLAG = 0;
                            return Flag.CRC_Err;
                        }
                    }
                    SEND_03_04_FLAG = 0;
                }
            }
            else
            {
                outtime_cnt++;
                emit Error_Tip_Signal(tr("Timeout Error"));
                uint outtime_num = MainWindow::outtime/40;
                if(outtime_cnt > outtime_num)
                {
                    SEND_03_04_FLAG = 0;
                    emit Data_Updata_Signal(-1);   //通讯中断
                }
                else if(outtime_cnt > 3)
                {
                    emit Data_Updata_Signal(-2);   //通讯异常
                }

            }
        }
    }
    return Flag.Func_Err;
}

int Serial::Write_10_Reg(uint ID,uint Address,QByteArray& v)
{
    QByteArray SEND_Buf;
    Modbus_Flag Flag;
    uint len = 0;
    switch (MainWindow::Clicked_Signal.data_type)
    {
        case 0:  //short
        case 1:  //ushort
            len = 1;
        break;
        case 2:  //int(abcd)
        case 3:  //int(cdab)
        case 4:  //int(badc)
        case 5:  //int(dcba)
            len = 2;
        break;
        case 6:  //uint(abcd)
        case 7:  //uint(cdab)
        case 8:  //uint(badc)
        case 9:  //uint(dcba)
            len = 2;
        break;
        case 10:  //float(abcd)
        case 11:  //float(cdab)
        case 12:  //float(badc)
        case 13:  //float(dcba)
            len = 2;
        break;
        case 14:   //string
            len = MainWindow::Clicked_Signal.offset;
        break;
        case 15:   //datetime(abcdef)
            if(LoginDialog::LowInTheFirst)//6寄存器格式的低字节版本
            {
                len = 6;
            }
            else//3寄存器格式
            {
                len = 3;
            }
        break;

    }
    if(SEND_10_FLAG == 0)                //询问指令未发送
    {
        SEND_Buf[0] = char(ID);
        SEND_Buf[1] = 0x10;
        SEND_Buf[2] = char(Address>>8);
        SEND_Buf[3] = char(Address&0xff);
        SEND_Buf[4] = char(len>>8);
        SEND_Buf[5] = char(len&0xff);
        SEND_Buf[6] = char((len*2)&0xff);
        for(uint s = 0; s < len; s++)
        {
            switch (MainWindow::Clicked_Signal.data_type)
            {
                case 0:  //short
                case 1:  //ushort
                {
                    if(LoginDialog::LowInTheFirst)
                    {
                        SEND_Buf[7] = v[1];
                        SEND_Buf[8] = v[0];
                    }
                    else
                    {
                        SEND_Buf[7] = v[0];
                        SEND_Buf[8] = v[1];
                    }

                }
                break;
                case 2:  //int(abcd)
                {
                    SEND_Buf[7+(2*s)] = v[0+(2*s)];
                    SEND_Buf[8+(2*s)] = v[1+(2*s)];
                }
                break;
                case 3:  //int(cdab)
                {
                    SEND_Buf[7+(2*s)] = v[2-(2*s)];
                    SEND_Buf[8+(2*s)] = v[3-(2*s)];
                }
                break;
                case 4:  //int(badc)
                {
                    SEND_Buf[7+(2*s)] = v[1+(2*s)];
                    SEND_Buf[8+(2*s)] = v[0+(2*s)];
                }
                break;
                case 5:  //int(dcba)
                {
                    SEND_Buf[7+(2*s)] = v[3-(2*s)];
                    SEND_Buf[8+(2*s)] = v[2-(2*s)];
                }
                break;
                case 6:  //uint(abcd)
                {
                    SEND_Buf[7+(2*s)] = v[0+(2*s)];
                    SEND_Buf[8+(2*s)] = v[1+(2*s)];
                }
                break;
                case 7:  //uint(cdab)
                {
                    SEND_Buf[7+(2*s)] = v[2-(2*s)];
                    SEND_Buf[8+(2*s)] = v[3-(2*s)];
                }
                break;
                case 8:  //uint(badc)
                {
                    SEND_Buf[7+(2*s)] = v[1+(2*s)];
                    SEND_Buf[8+(2*s)] = v[0+(2*s)];
                }
                break;
                case 9:  //uint(dcba)
                {
                    SEND_Buf[7+(2*s)] = v[3-(2*s)];
                    SEND_Buf[8+(2*s)] = v[2-(2*s)];
                }
                break;
                case 10:  //float(abcd)
                {
                    SEND_Buf[7+(2*s)] = v[0+(2*s)];
                    SEND_Buf[8+(2*s)] = v[1+(2*s)];
                }
                break;
                case 11:  //float(cdab)
                {
                    SEND_Buf[7+(2*s)] = v[2-(2*s)];
                    SEND_Buf[8+(2*s)] = v[3-(2*s)];
                }
                break;
                case 12:  //float(badc)
                {
                    SEND_Buf[7+(2*s)] = v[1+(2*s)];
                    SEND_Buf[8+(2*s)] = v[0+(2*s)];
                }
                break;
                case 13:  //float(dcba)
                {
                    SEND_Buf[7+(2*s)] = v[3-(2*s)];
                    SEND_Buf[8+(2*s)] = v[2-(2*s)];
                }
                break;
                case 14:   //string
                    SEND_Buf[7+(2*s)] = v[0+(2*s)];
                    SEND_Buf[8+(2*s)] = v[1+(2*s)];
                break;
                case 15:   //datetime(abcdef)
                    if(LoginDialog::LowInTheFirst)//6寄存器格式的低字节版本
                    {
                        SEND_Buf[7+(2*s)] = v[s];
                        SEND_Buf[8+(2*s)] = 0;
                    }
                    else//3寄存器格式
                    {
                        SEND_Buf[7+(2*s)] = v[0+(2*s)];
                        SEND_Buf[8+(2*s)] = v[1+(2*s)];
                    }
                break;
            }
        }
        uint len_temp = len*2+7;
        unsigned char temp_buf[50];
        memcpy(temp_buf,SEND_Buf,len_temp);
        SEND_Buf[len_temp] = char(CRC16(temp_buf,len_temp)>>8);
        SEND_Buf[len_temp+1] = char(CRC16(temp_buf,len_temp)&0xff);
        serial->write(SEND_Buf.constData(),len_temp+2);
        qDebug()<<SEND_Buf;
        SEND_10_FLAG = 1;
        Read_Buf.clear();
        START_RECEIVE_FLAG = 0;
        RECEIVE_FINISH_FLAG = 0;
        outtime_cnt = 0;
    }
    else                                      //询问指令发送完成，等待接收
    {
        if(serial->waitForReadyRead(40))   //40ms未接收到下一帧数据，即本帧数据接收完成
        {
            if(RECEIVE_FINISH_FLAG == 0)
            {
                START_RECEIVE_FLAG = 1;
                Read_Buf += serial->readAll();
            }
        }
        else      //超时未接收到回复数据
        {
            if(START_RECEIVE_FLAG == 1)
            {
                RECEIVE_FINISH_FLAG = 1;
                if(RECEIVE_FINISH_FLAG == 1)
                {

                    if(Read_Buf.length() >= 5)
                    {
                        if(Read_Buf.at(0) != char(ID))
                        {
                            emit Error_Tip_Signal(tr("Slave ID Error"));
                            emit Data_Updata_Signal(-2);   //通讯异常
                            //qDebug()<<"Flag.ID_Err";
                            SEND_10_FLAG = 0;
                            return Flag.ID_Err;
                        }
                        if(Read_Buf.at(1) != 0x10)
                        {
                            if(Read_Buf.at(1) == (char)0x90)
                            {
                                uint val = Read_Buf.at(2);
                                emit Error_Tip_Signal(tr("Error Num:%1").arg(val));
                            }
                            emit Data_Updata_Signal(-2);   //通讯异常
                            //qDebug()<<"Flag.Func_Err";
                            SEND_10_FLAG = 0;
                            return Flag.Func_Err;
                        }
                        int len = Read_Buf.length();
                        unsigned char temp_buf[100];
                        memcpy(temp_buf,Read_Buf,uint(len-2));
                        uint CRC = CRC16(temp_buf,uint(len-2));
                        if((Read_Buf.at(len-2) == char(CRC>>8))&&(Read_Buf.at(len-1) == char(CRC&0xff)))  //接收完成
                        {
                            //qDebug()<<"Flag.Success";
                            emit Error_Tip_Signal(tr(""));
                            SEND_10_FLAG = 0;
                            Write_FLAG = 0;
                            emit Data_Updata_Signal(-5);   //写入成功
                            return Flag.Success;
                        }
                        else
                        {
                            emit Data_Updata_Signal(-2);   //通讯异常
                            //qDebug()<<"Flag.CRC_Err";
                            SEND_10_FLAG = 0;
                            return Flag.CRC_Err;
                        }
                    }
                    SEND_10_FLAG = 0;
                }
            }
            else
            {
                outtime_cnt++;
                uint outtime_num = MainWindow::outtime/40;
                emit Error_Tip_Signal(tr("Timeout Error"));
                if(outtime_cnt > outtime_num)
                {
                    SEND_10_FLAG = 0;
                    emit Data_Updata_Signal(-1);   //通讯中断
                }
                else if(outtime_cnt > 3)
                {
                    emit Data_Updata_Signal(-2);   //通讯异常
                }

            }
        }
    }
    return Flag.Func_Err;
}

