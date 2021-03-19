#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <crc32.h>
#include <QSerialPortInfo>
#include <QFile>
#include<QDataStream>
#include<QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    d_num=0x63;

   QByteArray ba = QByteArray::fromHex(QVariant("ffffffffffb5630040a322").toByteArray());

   QByteArray my;
   my.append(d_num);
   my.append((quint8)0);
   my.append(0x40);
   my.append(get_CRC8(my));
   my.append(0x22);

   my.insert(0,0xb5);
   my.insert(0,0xFF);
   my.insert(0,0xFF);
   my.insert(0,0xFF);
   my.insert(0,0xFF);
   my.insert(0,0xFF);



   qDebug()<<ba.toHex();
   qDebug()<<my.toHex();

//Обнуление настроек
    quit=0;
    step=0;


//Нарисуй ком порты
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        ui->PortName->addItem(info.portName());



//Здесь закачай файл в буффер

QByteArray bin;
bin.clear();
QString filepath= QCoreApplication::applicationDirPath() + "/rif_encr.bin";
qDebug()<<"filepath "<<filepath;
    QFile file(filepath);

    if(file.open(QIODevice::ReadOnly))
    {
        //qDebug()<<"file.size "<<file.size();
        QDataStream stream(&file);    // read the data serialized from the file

        for(int i=0;i<file.size();i++)
        {
            quint8 dt;
            stream >> dt;
            bin.append(dt);
        }

    }

int cnt=0;
QByteArray line;
line.clear();
int block_number=0;
    for(int i=0;i<bin.size();i++)
    {
        line.append((quint8)bin.at(i));
        cnt++;
        if((cnt==32)||(cnt==bin.size()))
        {
          map.insert(block_number,line);
          block_number++;
          qDebug()<<line.toHex();
          line.clear();
          cnt=0;

        }

    }
    this->ui->progressBar->setMinimum(0);
    this->ui->progressBar->setMaximum(map.size());

 //  qDebug()<<"bin :"<<bin.toHex();
/*   qDebug()<<"---------------------------";
   qDebug()<<"-";
   qDebug()<<"-";
   qDebug()<<"-";
   qDebug()<<"-";
   qDebug()<<"-";
   qDebug()<<"---------------------------";

    //qDebug()<<"map size "<<map.size();
//    //qDebug()<<map.value(1).toHex();
//       //qDebug()<<map.value(2).toHex();
    for(int i=0;i<map.size()+1;i++)
    {
        qDebug()<<"-----------------------------------------------------------------------block "<<i;
        qDebug()<<map.value(i).toHex();

    }*/
QString testfilepath= QCoreApplication::applicationDirPath() + "/rif_encr_TEST.bin";

    QFile test(testfilepath);

    if(test.open(QIODevice::WriteOnly))
    {

        //qDebug()<<"file.size "<<file.size();
        QDataStream stream(&test);    // read the data serialized from the file

        for(int i=0;i<map.size()+1;i++)
        {
            for(int x=0;x<map.value(i).size();x++)
            {

                stream << (quint8)map.value(i).at(x);

            }

        }


    }


/*
 *
 *
 *
 *
 * /
//И разбей на блоки
/*
 *
 *
 *
 */

//И протести собери обратно в файл сохрани и сравни в Total Commander
/*
 *
 *
 *
 */



//Сигналы
    connect(&tmr_1,SIGNAL(timeout()),this,SLOT(tmr_1_timeout()));
    connect(&tmr_2,SIGNAL(timeout()),this,SLOT(tmr_2_timeout()));
    connect(&port, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(this, SIGNAL(data_from_port(QByteArray)), this, SLOT(slot_to_data_from_port(QByteArray)));
    connect(this, SIGNAL(take_block(QByteArray)), this, SLOT(get_block_to_test(QByteArray)));

    //


    //

}

MainWindow::~MainWindow()
{
    step=0;
    process();
    port.close();
    delete ui;

}







quint32 MainWindow::calcCRC32(QByteArray data)
{
    static const quint32 crc32Table[] =
    {
        0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
        0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
        0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
        0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
        0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
        0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
        0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
        0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
        0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
        0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
        0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
        0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
        0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
        0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
        0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
        0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
        0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
        0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
        0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
        0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
        0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
        0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
        0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
        0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
        0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
        0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
        0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
        0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
        0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
        0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
        0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
        0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
        0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
        0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
        0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
        0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
        0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
        0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
        0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
        0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
        0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
        0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
        0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
    };

    quint32 crc32 = 0xffffffffL;

    for (auto i = 0; i < data.size(); ++i ) {
        crc32 = crc32Table[ ( crc32 ^ data.constData()[ i ]) & 0xff ] ^ ( crc32 >> 8 );
    }

    return (crc32 ^ 0xffffffffL);
}

quint8 MainWindow::get_CRC8(QByteArray data)
{
    quint8 crc=0;
    for(int i=0;i<data.size();i++)
    {
        crc=crc+(quint8)data.at(i);
    }
    return crc;
}

QByteArray MainWindow::wrap_block(QByteArray block, int blok_number,int dev_number)
{
    QByteArray ba = QByteArray::fromHex(QVariant("b563224107080102030405060708091011121314151617181920212223242526272829303132").toByteArray());

    QByteArray raw=QByteArray::fromHex(QVariant("0102030405060708091011121314151617181920212223242526272829303132").toByteArray());


    QByteArray number;
    number.clear();
    number.append((quint8)(blok_number/0x100));
    number.append((quint8)(blok_number%0x100));

   // //qDebug()<<"номер "<<number.toHex();
    quint8 size=number.size()+block.size();
   // //qDebug()<<"размер "<<size;
    quint8 cmd=0x41;

    QByteArray res;
    res.clear();
    res.append(0xB5);
    res.append(dev_number);
    res.append(size);
    res.append(cmd);
    res.append(number);
    res.append(block);

    quint32 crc32 = Crc32::calcCRC32(ba);
  //  //qDebug()<<ba.toHex();
  //  //qDebug()<<res.toHex();
   // //qDebug()<<"crc32 raw"<<crc32;
    crc32 = Crc32::calcCRC32(res);
   // //qDebug()<<"crc32 res"<<crc32;

    QByteArray crc32_ba;
    crc32_ba.clear();
    crc32_ba.append((quint8)(crc32/0x1000000));
    crc32_ba.append((quint8)(crc32/0x10000));
    crc32_ba.append((quint8)(crc32/0x100));
    crc32_ba.append((quint8)(crc32%0x100));

     res.append(crc32_ba);

     return res;


     //qDebug()<<"crc32_ba "<<crc32_ba.toHex();
}



int MainWindow::getKvit()
{
    int val=kvit;
    kvit=0;
    return val;
}

void MainWindow::setKvit()
{
    kvit = 1;
}

void MainWindow::get_kvit_msg()
{

    //qDebug()<<"get_kvit; step "<<step;
    switch(step)
    {
    case 1:
        tmr_1.stop();
        step=2;
        process();
    break;

    }

}

void MainWindow::get_kvit_msg_with_block_number(int bl_nbr)
{
    tmr_2.stop();
    //qDebug()<<"get_kvit_with_block_number" ;
    switch(step)
    {
    case 2:

        if(bl_nbr==blk_nbr)
        {
           repeat=0;
           this->ui->progressBar->setValue(blk_nbr);
           blk_nbr++;
          if(blk_nbr==map.size())
         //   if(blk_nbr==5)
           {
            step=3;
            for(int i=1;i<map.size()+1;i++)
            {
          //      qDebug()<<"-----------------------------------------------------------------------block "<<i;
          //     qDebug()<<map.value(i).toHex();

            }

           }

        }
        else
        {
            repeat++;
            qDebug()<<"repeat "<<repeat;
            if(repeat>3)
            {
            QMessageBox::critical(0,"Помехи со связью",
                                  "Не принимает блок");

            step=0;
            }


        }
        process();

    break;

    }
}

void MainWindow::send_block_number(int nbr,int dev_number)
{
//    qDebug()<<"передаю "<<blk_nbr<<" из "<<map.size();

//QByteArray raw=QByteArray::fromHex(QVariant("1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF1").toByteArray());


QByteArray res=this->wrap_block(map.value(nbr),nbr,dev_number);

//if(nbr==3)
//res=this->wrap_block(map.value(nbr),7);



qDebug()<<"передаю "<<blk_nbr<<" из "<<map.size()<<" "<<res.toHex();

 port.readAll();
 port.write(res);
 port.waitForBytesWritten();

 //emit take_block(res);


}

// Get the crc32 code of the file
QString MainWindow::calcFileCRC(QString fileName)
{
    QString strCRC = "";
    quint32 crc = 0;;
    QFile file(fileName);
    bool result = file.open(QFile::ReadOnly);
    if(result) {
        while(!file.atEnd()) {
            QByteArray raw = file.read(1024*1024);
            crc = crc ^ calcCRC32(raw);
        }

        file.close();
        strCRC = QString::number(crc, 16);
    }

    return strCRC;
}


void MainWindow::on_OpenPort_clicked()
{


   port.close();
   port.setPortName(ui->PortName->currentText());
   port.setBaudRate(QSerialPort::Baud4800);
   port.setParity(QSerialPort::NoParity);
   port.setStopBits(QSerialPort::OneStop);



   if(port.open(QIODevice::ReadWrite))
   {
   this->ui->PortState->setText("открыт");
   //qDebug()<<"Порт "<<port.portName()<<" открыт";
   }
   else
   {
   this->ui->PortState->setText("закрыт");
   //qDebug()<<"Порт "<< port.portName()<<" закрыт";
   }

   ui->pushButton->setEnabled(port.isOpen());





}

void MainWindow::on_PortName_currentIndexChanged(const QString &arg1)
{
    port.close();
    this->ui->PortState->setText("закрыт");
    ui->pushButton->setEnabled(port.isOpen());
    step=0;
    process();
}

void MainWindow::on_pushButton_clicked()
{
    //qDebug()<<"--Обновление прошивки-----";

    step=0;
    process();
    step=1;
    process();

}

void MainWindow::load_file_to_buffer(QString filepath)
{

}

void MainWindow::tmr_1_timeout()//не дождался квитанцию
{
qDebug()<<"tmr1 timeout";
  tmr_1.stop();
  //qDebug()<<"tmr1_stop";

  count_1++;
  if(count_1<3)//Счетчик_1 не больше 10?
  {
      QEventLoop loop;
      QTimer::singleShot(2700, &loop, SLOT(quit()));
      loop.exec();
    process();//Повторить этот шаг.

  }
  else//Счетчик_1 больше 10?
  {
      //Вывести Нет Связи

  step=0;
  QMessageBox::critical(0,"Помехи со связью","Нет квитанции на старт");
  process();//Все параметры на исходную. Счетчик_1 равен нулю.
  }
/*  */



}

void MainWindow::tmr_2_timeout()
{
  qDebug()<<"tmr2 timeout";
  tmr_2.stop();
  count_2++;
  if(count_2<3)//Счетчик_1 не больше 10?
  {
    process();//Повторить этот шаг.

  }
  else//Счетчик_1 больше 10?
  {
      //Вывести Нет Связи
  step=0;
  QMessageBox::critical(0,"Помехи со связью","Нет квитанции на запись блока");
  process();//Все параметры на исходную. Счетчик_1 равен нулю.
  }

}

void MainWindow::readData()
{
 //   //qDebug()<<".";
    QByteArray data;
    data.clear();


  //  data.append(port.readAll());
    while(port.waitForReadyRead(20))
    data.append(port.readAll());


    emit data_from_port(data);
    /*
    if(port.waitForReadyRead())
    {
        data.append(port.readAll());
        if(port.waitForReadyRead(10))
        {
            data.append(port.readAll());


        }

            //qDebug()<<data.toHex();
    }
*/


}

void MainWindow::slot_to_data_from_port(QByteArray data)
{
   // qDebug()<<"!";
    qDebug()<<"принял: "<<data.toHex();
//Анализ принятого сообщения


//Ждем B5
    QByteArray res;
    quint8 crc=0;
    int B5=0;
    int cnt=0;
    int size=0;
    for(int i=0;i<data.size();i++)
    {
        //Определяем начало посылки
        if(0==B5)
        {
            if(0xB5==(quint8)data.at(i))
            {
             //qDebug()<<"Стартбайт";
             B5=1;
             cnt=0;
             size=4;
            }
        }
        else
        {
            if(cnt<size)
            {
                res.append((quint8)data.at(i));

                if(3==cnt)
                {
                size=(quint8)data.at(i)+5;
                //qDebug()<<"Размер "<<size;
                }

                if(4==cnt)
                {
                int cmd=(quint8)data.at(i);
                if(0x30==cmd)
                    {
                    //    //qDebug()<<"[PROFIT]";


                    }

                }
            }
            else
            {
                if(cnt==size)
                {


                    //qDebug()<<"посылка: "<<res.toHex();
                    crc=(quint8)data.at(i);
                    //qDebug()<<"crc "<<crc;
                    quint8 real_crc=0;

                    for(int x=0;x<res.size();x++)
                    {
                       real_crc=real_crc+(quint8)res.at(x);
                    }

                    //qDebug()<<"real_crc "<<real_crc;
                    if(crc==real_crc)
                    {
                        //qDebug()<<"[PROFIT]";
                        //qDebug()<<size;
                        if(size==5)
                        {
                        get_kvit_msg();
                        }
                        else if(size==7)
                        {
                        int bl_nbr=(quint8)res.at(4)*256+(quint8)res.at(5);
                        //qDebug()<<"номер блока "<<bl_nbr;
                        get_kvit_msg_with_block_number(bl_nbr);
                        }

                    }

                      i=data.size();
/**/
                }

            }




        }





        cnt++;

    }
}

void MainWindow::get_block_to_test(QByteArray data)
{
    qDebug()<<"----собираем-------------------------------";
    qDebug()<<data.toHex();
    QByteArray res;
    quint8 crc=0;
    int B5=0;
    int cnt=0;
    int size=0;
    for(int i=0;i<data.size();i++)
    {
        //Определяем начало посылки
        if(0==B5)
        {
            if(0xB5==(quint8)data.at(i))
            {
             qDebug()<<"Стартбайт";
             B5=1;
             cnt=0;
             size=4;
            }
        }
        else
        {
            if(cnt<size)
            {
                if(cnt>5)
                res.append((quint8)data.at(i));

                if(2==cnt)
                {
                qDebug()<<"(quint8)data.at(i)"<<(quint8)data.at(i);
                size=(quint8)data.at(i)+4;
                qDebug()<<"Размер "<<size;
                }

                if(4==cnt)
                {
                int cmd=(quint8)data.at(i);
                if(0x30==cmd)
                    {
                        qDebug()<<"[PROFIT]";


                    }

                }
            }
            else
            {
                if(cnt==size)
                {


                    qDebug()<<"посылка: "<<res.toHex();
                    map_r.insert(blk_nbr,res);

                    /*
                    crc=(quint8)data.at(i);
                    //qDebug()<<"crc "<<crc;
                    quint8 real_crc=0;

                    for(int x=0;x<res.size();x++)
                    {
                       real_crc=real_crc+(quint8)res.at(x);
                    }

                    //qDebug()<<"real_crc "<<real_crc;
                    if(crc==real_crc)
                    {
                        //qDebug()<<"[PROFIT]";
                        //qDebug()<<size;
                        if(size==5)
                        {
                        get_kvit_msg();
                        }
                        else if(size==7)
                        {
                        int bl_nbr=(quint8)res.at(4)*256+(quint8)res.at(5);
                        //qDebug()<<"номер блока "<<bl_nbr;
                        get_kvit_msg_with_block_number(bl_nbr);
                        }

                    }

                      i=data.size();

                    */
                }

            }




        }





        cnt++;

    }
qDebug()<<"----собираем-------------------------------";
}

void MainWindow::process()
{
    qDebug()<<"["<<step<<"]";
    switch(step)
    {
    case 0:

      //  blk_nbr=1468;
        blk_nbr=0;
        repeat=0;
        getKvit();
        count_1=0;
        count_2=0;
        tmr_1.stop();
        tmr_2.stop();
        map_r.clear();
        this->ui->progressBar->setValue(0);
    break;

    case 1:

// Отправить команду Старт
    cmd_start();

    tmr_1.start(1000);
//Начать ждать квитанцию столько то времени

//Дождался квитанцию? перейти на шаг 2

//Не дождался квитанцию?


//Прибавить счетчик_1





    break;

    case 2:
    //qDebug()<<"step 2";
//Завернуть первый блок в обертку
//(Написать функцию завернуть блок номер такой то в обертку)
    send_block_number(blk_nbr,d_num);

    tmr_2.start(300);
//Отправить
//Начать ждать квитанцию
    break;

    case 3:

      //  cmd_finish();
       //  QMessageBox::critical(0,"Ошибка","Такой обьект уже существует");
        this->ui->progressBar->setValue(this->ui->progressBar->maximum());

        QMessageBox::information(0,"","Обновление прошло успешно") ;
                step=4;
  //  tmr_1.start(1000);
     //   tmr_1.start(1000);

    break;

    case 4:

    break;

    default:

    break;



    }
}

void MainWindow::cmd_start()
{
   //qDebug()<<"cmd start";


 //  QByteArray ba = QByteArray::fromHex(QVariant("ffffffffffb5630040a322").toByteArray());

   QByteArray my;
   my.append(d_num);
   my.append((quint8)0);
   my.append(0x40);
   my.append(get_CRC8(my));
   my.append(0x22);

   my.insert(0,0xb5);
   my.insert(0,0xFF);
   my.insert(0,0xFF);
   my.insert(0,0xFF);
   my.insert(0,0xFF);
   my.insert(0,0xFF);
   port.write(my);
   port.waitForBytesWritten();
}

void MainWindow::cmd_finish()
{
    QByteArray ba = QByteArray::fromHex(QVariant("ffffffffffb5630042a522").toByteArray());
  /*
    QByteArray res;
    res.append(0xFF);
    res.append(0xFF);
    res.append(0xFF);
    res.append(0xFF);
    res.append(0xFF);
    res.append(ba);
    res.append(0x22);
 */
    port.write(ba);
    port.waitForBytesWritten();
}
