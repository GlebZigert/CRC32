#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <crc_toolchain.h>
#include <QSerialPort>
#include <QTimer>
#include<QMap>
#include<QTextStream>
#include<QList>





QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:



    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    quint8 d_num;

    int getKvit();
    void setKvit();

private slots:

    void on_OpenPort_clicked();
    void on_PortName_currentIndexChanged(const QString &arg1);

    void on_pushButton_clicked();
    void load_file_to_buffer(QString filepath);





    void on_dev_num_currentIndexChanged(const QString &arg1);

    void on_open_bin_file_clicked();

    void on_get_version_clicked();



    void on_action_triggered();

public slots:

    void tmr_1_timeout();
    void tmr_2_timeout();
    void readData();
    void slot_to_data_from_port(QByteArray data);
    void get_block_to_test(QByteArray);
private:
    QMap<int,QByteArray> map;
    QMap<int,QByteArray> map_r;

    QByteArray data;

    /*
    QByteArray data{
    0xb5, 0x63, 0x22, 0x41, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14,
    0x15, 0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29 ,0x30 ,0x31 ,0x32 ,0x8b, 0x40
    ,0x12 ,0xf8
    };*/

    int repeat;

    void process();



    int count_1;
    int count_2;

    void cmd_start();
    void cmd_get_version();
    void cmd_finish();

    QTimer tmr_1;
    QTimer tmr_2;
    QTimer tmr_on_rcv;
    Ui::MainWindow *ui;
    bool quit;
    QSerialPort port;
    QString calcFileCRC(QString fileName);
    quint32 calcCRC32(QByteArray data);

    quint8 get_CRC8(QByteArray data);

    QTextStream* log_stream;

    QList<int> crc_list;

    int kvit;

    int step;

    int blk_nbr;

    QByteArray wrap_block(QByteArray block, int block_number,int dev_number);

    int add_bin_to_bin_ini();

    void get_kvit_msg();
    void get_kvit_msg_with_block_number(int blk_nbr);

    void send_block_number(int nbr,int dev_number);

    void get_version(QByteArray data);



signals:
    void data_from_port(QByteArray data);
    void take_block(QByteArray);
};
#endif // MAINWINDOW_H
