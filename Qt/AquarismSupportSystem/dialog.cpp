#include "dialog.h"
#include "ui_dialog.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <string>
#include <QDebug>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->tempLcdNumber->display("-------");

    arduino = new QSerialPort(this);

    QFont formation("Arial",18);
    ui->Max_Temp_Def->setFont(formation);
    ui->Min_Temp_Def->setFont(formation);
    ui->Date_Value->setFont(formation);
    ui->Time_Value->setFont(formation);
    ui->Max_Temp_Def->setText("30");
    ui->Min_Temp_Def->setText("27");
    updateTemperatureMessage();

    clean_days  = 30;
    clean_hours = 0;
    feed_days   = 1;
    feed_hours  = 0;

    refreshConnection();
}



Dialog::~Dialog()
{
    delete ui;
}

void Dialog::refreshConnection()
{
    serialBuffer = "";

    qDebug() << "Number of ports: " << QSerialPortInfo::availablePorts().length() << "\n";
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        qDebug() << "Description: "    << serialPortInfo.description()          << "\n";
        qDebug() << "Has vendor ID? "  << serialPortInfo.hasVendorIdentifier()  << "\n";
        qDebug() << "Vendor ID: "      << serialPortInfo.vendorIdentifier()     << "\n";
        qDebug() << "Has product ID? " << serialPortInfo.hasProductIdentifier() << "\n";
        qDebug() << "Product ID: "     << serialPortInfo.productIdentifier()    << "\n";
    }

    //identify if the arduino uno port is on:
    bool arduino_available = false;
    QString arduino_port_name;

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
            if((serialPortInfo.vendorIdentifier()==arduino_uno_vendor_id) && (serialPortInfo.productIdentifier()==arduino_uno_product_id)){
                arduino_available = true;
                arduino_port_name = serialPortInfo.portName();
            }
        }
    }

    if(arduino_available){
        qDebug() << "Found the arduino port...\n";
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::ReadOnly);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));
    }else{
        qDebug() << "Couldn't find the correct port for arduino.\n";
        QMessageBox::information(this,"Serial Port Error", "Couldn't open serial port to Arduino Uno.");
    }
}

void Dialog::readSerial()
{
    qDebug() << "Serialport works\n";

   // serialData.clear();
   // arduino->clear();

    serialData = arduino->readAll();
    serialBuffer = QString::fromStdString( serialData.toStdString() );
    qDebug() << serialBuffer;

    QStringList bufferSplit = serialBuffer.split("|");

    if(bufferSplit[0].split(":")[0] == "T"){
        QString temperature = bufferSplit[0].split(":")[1];
        qDebug() << temperature;
        updateLCD(temperature);
    }else{
        //get Date
        QString date = bufferSplit[0].split(":")[1];
        QString week = bufferSplit[1].split(":")[1];
        QString hour = bufferSplit[2].split(":")[1].split("\r\n")[0];

        QString day = QStringLiteral("%1").arg( date.split("/")[0].toInt() , 2, 10, QLatin1Char('0'));
        QString mon = QStringLiteral("%1").arg( date.split("/")[1].toInt() , 2, 10, QLatin1Char('0'));
        QString year = QStringLiteral("%1").arg( date.split("/")[2].toInt() , 4, 10, QLatin1Char('0'));

        QString h = QStringLiteral("%1").arg( hour.split("/")[0].toInt() , 2, 10, QLatin1Char('0'));
        QString m = QStringLiteral("%1").arg( hour.split("/")[1].toInt() , 2, 10, QLatin1Char('0'));

        update_date_values(day,mon,year);
        update_time_values(h,m);
        ui->WeekDay->setText(week);

        qDebug() << date;
        qDebug() << week;
        qDebug() << hour;
        serialData.clear();
    }
}



void Dialog::updateLCD(const QString sensor_reading){
    ui->tempLcdNumber->display(sensor_reading);
    updateTemperatureMessage();
}

void Dialog::updateTemperatureMessage()
{
    float disp_msg = ui->tempLcdNumber->value();
    QString minus = "-------";
    float trying = minus.toFloat();

    if(disp_msg == trying){
        ui->TemperatureInfo->setStyleSheet("color : red");
        ui->TemperatureInfo->setText("No temperature info.");
    }else if(disp_msg <= ui->Max_Temp_Def->text().toFloat()
             && disp_msg >= ui->Min_Temp_Def->text().toFloat()){
        ui->TemperatureInfo->setStyleSheet("color : black");
        ui->TemperatureInfo->setText("The temperature is within the standards.");
    }else{
        ui->TemperatureInfo->setStyleSheet("color : red");
        ui->TemperatureInfo->setText("The temperature is NOT within the standards.");
    }
}

void Dialog::on_EditTemperature_clicked()
{
    Edit_Temperature_Dialog *subdialog = new Edit_Temperature_Dialog(this);
    subdialog->setFixedSize(344,126);
    subdialog->setWindowTitle("Edit Maximum and Minimum Temperatures");
    qDebug() << ui->Max_Temp_Def->text() << "\n";
    qDebug() << ui->Min_Temp_Def->text() << "\n";
    QString max_temp_s = ui->Max_Temp_Def->text();
    QString min_temp_s = ui->Min_Temp_Def->text();

    float maxtemp = max_temp_s.toFloat();
    float mintemp = min_temp_s.toFloat();

    subdialog->set_Max_Min_Value(maxtemp,mintemp);
    subdialog->show();
    subdialog->set_Max_Min_Value(maxtemp,mintemp);
    qDebug() << max_temp_s << " " << min_temp_s << "\n";

}

void Dialog::update_temperature_values(QString max, QString min)
{

    ui->Max_Temp_Def->setText(max);
    ui->Min_Temp_Def->setText(min);

    qDebug() << ui->Max_Temp_Def->text() << " " << ui->Min_Temp_Def->text() << "\n";

    updateTemperatureMessage();
}

void Dialog::update_date_values(QString day, QString mon, QString yea)
{
    QString value = day + "/" + mon + "/" + yea;
    ui->Date_Value->setText(value);
}

void Dialog::update_time_values(QString hour, QString min)
{
    QString value = hour + ":" + min;
    ui->Time_Value->setText(value);
}

void Dialog::on_pushButton_clicked( )
{
    refreshConnection();
}


void Dialog::on_Set_Clean_Period_clicked()
{
    clean_days  = ui->Days_Clean_Value->value();
    clean_hours = ui->Hours_Clean_Value->value();
}

void Dialog::on_Set_Feed_Period_clicked()
{
    feed_days  = ui->Days_Feed_Value->value();
    feed_hours = ui->Hours_Feed_Value->value();
}
