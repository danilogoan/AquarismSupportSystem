#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include "edit_temperature_dialog.h"

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
    void refreshConnection(Dialog *Di);
    void readSerial();
    void updateLCD(const QString);
    void updateTemperatureMessage();

    void on_EditTemperature_clicked();
    void update_temperature_values(QString,QString);

    void on_pushButton_clicked(QDialog *Di);

private:
    Ui::Dialog *ui;

    QSerialPort *arduino;
    static const quint16 arduino_uno_vendor_id = 6790;
    static const quint16 arduino_uno_product_id = 29987;

    QByteArray serialData;
    QString serialBuffer;
};

#endif // DIALOG_H
