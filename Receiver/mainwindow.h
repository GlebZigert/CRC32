#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <crc_toolchain.h>
#include <QSerialPort>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_OpenPort_clicked();
    void on_PortName_currentIndexChanged(const QString &arg1);

    void on_pushButton_clicked();



public slots:

    void tmr_1_timeout();
    void readData();
    void slot_to_data_from_port(QByteArray);
    void get_block_to_test(QByteArray block);
private:

    void process(int step);
    int count_1;

    void cmd_start();
    QTimer tmr_1;
    Ui::MainWindow *ui;
    bool quit;
    QSerialPort port;
    QString calcFileCRC(QString fileName);
    quint32 calcCRC32(QByteArray data);

signals:
    void data_from_port(QByteArray data);
    void take_block(QByteArray block);
};
#endif // MAINWINDOW_H
