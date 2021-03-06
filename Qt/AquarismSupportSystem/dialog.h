#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include "edit_temperature_dialog.h"
#include "connection.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void refreshConnection();
    void readSerial(QByteArray);
    void writeSerial();
    void decode();
    void updateLCD(const QString);
    void updateTemperatureMessage();

    void on_EditTemperature_clicked();
    void update_temperature_values(QString,QString);
    void update_ids(quint16,quint16);
    void update_date_values(QString, QString, QString);
    void update_time_values(QString, QString);

    void on_pushButton_clicked();

    void on_Set_Clean_Period_clicked();

    void on_Set_Feed_Period_clicked();

    void on_pushButton_Clean_clicked();
    void update_Clean_Message();

    void on_pushButton_Feed_clicked();
    void update_Feed_Message();

    void refresh_next_feed_time();
    void refresh_next_clean_time();

signals:
    void messsageReceived(QByteArray message);

private:
    Ui::Dialog *ui;

    QSerialPort *arduino;
    quint16 arduino_uno_vendor_id = 6790;
    quint16 arduino_uno_product_id = 29987;

    QByteArray serialData;
    QByteArray m_buffer;
    QString serialBuffer;

    int clean_days;
    int clean_hours;
    int feed_days;
    int feed_hours;

    bool arduino_connected;

    int food_alarm;
    int clean_alarm;
    int temp_alarm;
    int hit_median;
    int rising;
};

#endif // DIALOG_H
